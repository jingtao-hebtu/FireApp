/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : InferenceORT.cpp
   Author : tao.jing
   Date   : 2024/11/17
   Brief  :
**************************************************************************/
#include "InferenceORT.h"
#include "DetectDef.h"
#include "TLog.h"
#include "TConfig.h"
#include "DetectManager.h"
#include <onnxruntime_cxx_api.h>
#include <regex>


#ifdef USE_CUDA
#include <cuda_runtime.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include <io.h>
#endif


#ifdef min
#undef min
#endif
#define min(a, b)            (((a) < (b)) ? (a) : (b))


TF::InferenceORT::InferenceORT(const std::string &model_path) {

    DL_INIT_PARAM params;
    params.rectConfidenceThreshold = GET_FLOAT_CONFIG("VisionMea", "RectConfidenceThreshold");
    params.iouThreshold = GET_FLOAT_CONFIG("VisionMea", "IouThreshold");;
    params.modelPath = model_path;
    params.imgSize = {TF_DETECT_IMG_SIZE, TF_DETECT_IMG_SIZE};
#ifdef USE_CUDA
    params.cudaEnable = true;

    // GPU FP32 inference
    //params.modelType = YOLO_DETECT;
    params.modelType = YOLO_SEG;
    // GPU FP16 inference
    //Note: change fp16 onnx model
    //params.modelType = YOLO_DETECT_HALF;
#else
    // CPU inference
    params.modelType = YOLO_DETECT;
    params.cudaEnable = false;
#endif

    CreateSession(params);
}

TF::InferenceORT::~InferenceORT() {

}

#ifdef USE_CUDA
namespace Ort {
    template<>
    struct TypeToTensorType<half> {
        static constexpr ONNXTensorElementDataType type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16;
    };
}
#endif


template<typename T>
void BlobFromImage(cv::Mat &iImg, T &iBlob) {
    int channels = iImg.channels();
    int imgHeight = iImg.rows;
    int imgWidth = iImg.cols;

    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < imgHeight; h++) {
            for (int w = 0; w < imgWidth; w++) {
                iBlob[c * imgWidth * imgHeight + h * imgWidth + w] = typename std::remove_pointer<T>::type(
                        (iImg.at<cv::Vec3b>(h, w)[c]) / 255.0f);
            }
        }
    }
}

#ifdef USE_CUDA
void PrepareInputTensor(cv::Mat& matImg, Ort::Value& inputTensor, int width, int height) {
    // Step 1: 在 CPU 上进行预处理
    cv::Mat resizedImg;
    cv::resize(matImg, resizedImg, cv::Size(width, height));
    resizedImg.convertTo(resizedImg, CV_32F, 1.0 / 255.0);

    // Step 2: 调整数据形状，将 [H, W, C] 转换为 [C, H, W]
    std::vector<cv::Mat> channels(3);
    cv::split(resizedImg, channels);

    // 创建一个连续的内存块存储数据
    size_t dataSize = 1 * 3 * height * width * sizeof(float);
    float* cpuInputData = new float[dataSize / sizeof(float)];
    float* dataPtr = cpuInputData;

    // 将数据从 channels 填充到 cpuInputData
    for (int c = 0; c < 3; ++c) {
        memcpy(dataPtr, channels[c].data, width * height * sizeof(float));
        dataPtr += width * height;
    }

    // Step 3: 在 GPU 上分配内存
    float* devicePtr = nullptr;
    cudaError_t cudaStatus = cudaMalloc(&devicePtr, dataSize);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaMalloc failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        delete[] cpuInputData;
        return;
    }

    // Step 4: 将数据从 CPU 复制到 GPU
    cudaStatus = cudaMemcpy(devicePtr, cpuInputData, dataSize, cudaMemcpyHostToDevice);
    if (cudaStatus != cudaSuccess) {
        std::cerr << "cudaMemcpy failed: " << cudaGetErrorString(cudaStatus) << std::endl;
        cudaFree(devicePtr);
        delete[] cpuInputData;
        return;
    }

    // Step 5: 创建 ONNX Runtime 输入张量
    Ort::MemoryInfo memoryInfo("Cuda", OrtAllocatorType::OrtDeviceAllocator, 0, OrtMemTypeDefault);
    std::vector<int64_t> inputShape = {1, 3, height, width};

    inputTensor = Ort::Value::CreateTensor<float>(
            memoryInfo,
            devicePtr,                           // 您自行分配的 GPU 内存指针
            dataSize / sizeof(float),            // 元素数量
            inputShape.data(),
            inputShape.size()
    );

    // 清理 CPU 内存
    delete[] cpuInputData;
}
#endif


void TF::InferenceORT::PreProcess(cv::Mat &iImg, std::vector<int> iImgSize, cv::Mat &oImg) {
    mResizeScales = iImg.cols / (float) iImgSize.at(0);
    mResizeScaleX = static_cast<float>(mOriginalImgSize.width)  / iImgSize.at(0); // W0 / 640
    mResizeScaleY = static_cast<float>(mOriginalImgSize.height) / iImgSize.at(1); // H0 / 640
    cv::resize(iImg, oImg, cv::Size(iImgSize.at(0), iImgSize.at(1)));
}


bool TF::InferenceORT::CreateSession(DL_INIT_PARAM &iParams) {
    std::regex pattern("[\u4e00-\u9fa5]");
    bool result = std::regex_search(iParams.modelPath, pattern);
    if (result) {
        LOG_F(ERROR, "Your model path is error.Change your model path without chinese characters.");
        return false;
    }
    try {
        rectConfidenceThreshold = iParams.rectConfidenceThreshold;
        iouThreshold = iParams.iouThreshold;
        imgSize = iParams.imgSize;
        modelType = iParams.modelType;
        mEnv = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "Yolo");
        Ort::SessionOptions sessionOption;
        if (iParams.cudaEnable) {
            cudaEnable = iParams.cudaEnable;
            OrtCUDAProviderOptions cudaOption;
            cudaOption.device_id = 0;
            sessionOption.AppendExecutionProvider_CUDA(cudaOption);
        }
        sessionOption.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
        sessionOption.SetIntraOpNumThreads(iParams.intraOpNumThreads);
        sessionOption.SetLogSeverityLevel(iParams.logSeverityLevel);

#ifdef _WIN32
        int ModelPathSize = MultiByteToWideChar(CP_UTF8, 0, iParams.modelPath.c_str(),
                                                static_cast<int>(iParams.modelPath.length()), nullptr, 0);
        wchar_t *wide_cstr = new wchar_t[ModelPathSize + 1];
        MultiByteToWideChar(CP_UTF8, 0, iParams.modelPath.c_str(), static_cast<int>(iParams.modelPath.length()),
                            wide_cstr, ModelPathSize);
        wide_cstr[ModelPathSize] = L'\0';
        const wchar_t *modelPath = wide_cstr;
#else
        const char* modelPath = iParams.modelPath.c_str();
#endif // _WIN32

        mSession = new Ort::Session(mEnv, modelPath, sessionOption);
        Ort::AllocatorWithDefaultOptions allocator;
        size_t inputNodesNum = mSession->GetInputCount();
        for (size_t i = 0; i < inputNodesNum; i++) {
            Ort::AllocatedStringPtr input_node_name = mSession->GetInputNameAllocated(i, allocator);
            char *temp_buf = new char[50];
            strcpy(temp_buf, input_node_name.get());
            inputNodeNames.push_back(temp_buf);
        }
        size_t OutputNodesNum = mSession->GetOutputCount();
        for (size_t i = 0; i < OutputNodesNum; i++) {
            Ort::AllocatedStringPtr output_node_name = mSession->GetOutputNameAllocated(i, allocator);
            char *temp_buf = new char[10];
            strcpy(temp_buf, output_node_name.get());
            outputNodeNames.push_back(temp_buf);
        }
        options = Ort::RunOptions{nullptr};

        auto out0_info = mSession->GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo();
        auto out0_shape = out0_info.GetShape();  // [1, no, N]
        int64_t no = out0_shape[1];

        if (modelType == YOLO_SEG || modelType == YOLO_SEG_HALF) {
            // 第 1 个输出是 proto: [1, nm, H, W]
            auto out1_info = mSession->GetOutputTypeInfo(1).GetTensorTypeAndShapeInfo();
            auto out1_shape = out1_info.GetShape();
            nm_ = static_cast<int>(out1_shape[1]);              // nm
            nc_ = static_cast<int>(no - 4 - nm_);               // 4 + nc + nm = no
        } else {
            nm_ = 0;
            nc_ = static_cast<int>(no - 4);                     // 4 + nc = no
        }

        LOG_F(INFO, "Model head: no=%lld, nc=%d, nm=%d",
              static_cast<long long>(no), nc_, nm_);

        if (nc_ != static_cast<int>(classes.size())) {
            LOG_F(WARNING,
                  "classes.size() = %zu 与模型 nc = %d 不一致，"
                  "推理时将按 nc 进行切片，classes 仅用于可视化名称。",
                  classes.size(), nc_);
        }

        WarmUpSession();
        return true;
    }
    catch (const std::exception &e) {
        LOG_F(ERROR, "Create session failed %s.", e.what());
        return false;
    }
}

void TF::InferenceORT::RunSession(cv::Mat &iImg, std::vector<DL_RESULT> &oResult) {
    mOriginalImgSize = iImg.size();
    if (modelType == YOLO_DETECT || modelType == YOLO_POSE || modelType == YOLO_CLS || modelType == YOLO_SEG) {
#ifdef USE_CUDA
        Ort::Value inputTensor {nullptr};
        PrepareInputTensor(iImg, inputTensor, TF_DETECT_IMG_SIZE, TF_DETECT_IMG_SIZE);
        mResizeScales = iImg.cols / (float) imgSize.at(0);
        mResizeScaleX = static_cast<float>(mOriginalImgSize.width)  / imgSize.at(0);
        mResizeScaleY = static_cast<float>(mOriginalImgSize.height) / imgSize.at(1);
        std::vector<int64_t> inputNodeDims = {1, 3, imgSize.at(0), imgSize.at(1)};
        TensorProcess(inputTensor, inputNodeDims, oResult);
#else
        cv::Mat processedImg;
        PreProcess(iImg, imgSize, processedImg);
        float *blob = new float[processedImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> inputNodeDims = {1, 3, imgSize.at(0), imgSize.at(1)};
        TensorProcess(blob, inputNodeDims, oResult);
#endif
    } else {
#ifdef USE_CUDA
        cv::Mat processedImg;
        PreProcess(iImg, imgSize, processedImg);
        half *blob = new half[processedImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> inputNodeDims = {1, 3, imgSize.at(0), imgSize.at(1)};
        TensorProcess(blob, inputNodeDims, oResult);
#endif
    }
}


template<typename N>
bool TF::InferenceORT::TensorProcess(N &blob, std::vector<int64_t> &inputNodeDims,
                                       std::vector<DL_RESULT> &oResult) {
    Ort::Value inputTensor = Ort::Value::CreateTensor<typename std::remove_pointer<N>::type>(
            Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU), blob, 3 * imgSize.at(0) * imgSize.at(1),
            inputNodeDims.data(), inputNodeDims.size());

    auto outputTensor = mSession->Run(options, inputNodeNames.data(), &inputTensor, 1, outputNodeNames.data(),
                                      outputNodeNames.size());

    Ort::TypeInfo typeInfo = outputTensor.front().GetTypeInfo();
    auto tensor_info = typeInfo.GetTensorTypeAndShapeInfo();
    std::vector<int64_t> outputNodeDims = tensor_info.GetShape();
    auto output = outputTensor.front().GetTensorMutableData<typename std::remove_pointer<N>::type>();
    delete[] blob;
    switch (modelType) {
        case YOLO_DETECT:
        case YOLO_DETECT_HALF:
        case YOLO_SEG:
        case YOLO_SEG_HALF: {
            int signalResultNum = outputNodeDims[1];//84
            int strideNum = outputNodeDims[2];//8400
            std::vector<int> class_ids;
            std::vector<float> confidences;
            std::vector<cv::Rect> boxes;
            std::vector<cv::Mat> masks;
            cv::Mat rawData;
            if (modelType == YOLO_DETECT || modelType == YOLO_SEG) {
                // FP32
                rawData = cv::Mat(signalResultNum, strideNum, CV_32F, output);
            } else {
                // FP16
                rawData = cv::Mat(signalResultNum, strideNum, CV_16F, output);
                rawData.convertTo(rawData, CV_32F);
            }
            rawData = rawData.t();
            float *data = (float *) rawData.data;

            cv::Mat protoData;
            int protoHeight = 0;

            if (outputTensor.size() > 1 && (modelType == YOLO_SEG || modelType == YOLO_SEG_HALF)) {
                auto protoInfo = outputTensor[1].GetTensorTypeAndShapeInfo();
                auto protoShape = protoInfo.GetShape();
                protoHeight = static_cast<int>(protoShape[2]);
                protoData = cv::Mat(static_cast<int>(protoShape[1]), protoHeight * static_cast<int>(protoShape[3]), CV_32F,
                                    outputTensor[1].GetTensorMutableData<float>());
            }

            for (int i = 0; i < strideNum; ++i) {
                float *classesScores = data + 4;
                //cv::Mat scores(1, this->classes.size(), CV_32FC1, classesScores);
                cv::Mat scores(1, nc_, CV_32FC1, classesScores);
                cv::Point class_id;
                double maxClassScore;
                cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);
                if (maxClassScore > rectConfidenceThreshold) {
                    confidences.push_back(maxClassScore);
                    class_ids.push_back(class_id.x);
                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left   = static_cast<int>((x - 0.5f * w) * mResizeScaleX);
                    int top    = static_cast<int>((y - 0.5f * h) * mResizeScaleY);
                    int width  = static_cast<int>(w * mResizeScaleX);
                    int height = static_cast<int>(h * mResizeScaleY);

                    boxes.push_back(cv::Rect(left, top, width, height));

                    if (!protoData.empty()) {
                        //int maskStart = 4 + static_cast<int>(this->classes.size());
                        int maskStart = 4 + nc_;
                        int maskDim = signalResultNum - maskStart;
                        if (maskDim > 0) {
                            cv::Mat maskCoef(1, maskDim, CV_32F, data + maskStart);
                            cv::Mat mask = maskCoef * protoData;
                            mask = mask.reshape(1, protoHeight);
                            mask = sigmoid(mask);

                            cv::resize(mask, mask, cv::Size(imgSize.at(0), imgSize.at(1)));
                            cv::resize(mask, mask, mOriginalImgSize);

                            cv::Rect roi = boxes.back() & cv::Rect(0, 0, mask.cols, mask.rows);
                            cv::Mat boxMask = cv::Mat::zeros(mOriginalImgSize, CV_8UC1);
                            if (roi.width > 0 && roi.height > 0) {
                                cv::Mat maskCrop = mask(roi) > 0.5;
                                maskCrop.convertTo(maskCrop, CV_8UC1, 255.0);
                                maskCrop.copyTo(boxMask(roi));
                            }
                            masks.push_back(boxMask);
                        }
                    }
                }
                data += signalResultNum;
            }
            std::vector<int> nmsResult;
            cv::dnn::NMSBoxes(boxes, confidences, rectConfidenceThreshold, iouThreshold, nmsResult);
            for (int i = 0; i < nmsResult.size(); ++i) {
                int idx = nmsResult[i];
                DL_RESULT result;
                result.classId = class_ids[idx];
                result.confidence = confidences[idx];
                result.box = boxes[idx];
                if (!masks.empty() && idx < masks.size()) {
                    result.mask = masks[idx];
                }
                oResult.push_back(result);
            }
            break;
        }
        case YOLO_CLS:
        case YOLO_CLS_HALF: {
            cv::Mat rawData;
            if (modelType == YOLO_CLS) {
                // FP32
                rawData = cv::Mat(1, this->classes.size(), CV_32F, output);
            } else {
                // FP16
                rawData = cv::Mat(1, this->classes.size(), CV_16F, output);
                rawData.convertTo(rawData, CV_32F);
            }
            float *data = (float *) rawData.data;

            DL_RESULT result;
            for (int i = 0; i < this->classes.size(); i++) {
                result.classId = i;
                result.confidence = data[i];
                oResult.push_back(result);
            }
            break;
        }
        default:
            LOG_F(ERROR, "Not support model type.");
    }
    return true;
}


void TF::InferenceORT::WarmUpSession() {
    cv::Mat iImg = cv::Mat(cv::Size(imgSize.at(0), imgSize.at(1)), CV_8UC3);
    cv::Mat processedImg;
    PreProcess(iImg, imgSize, processedImg);
    if (modelType < 5) {
        float *blob = new float[iImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> YOLO_input_node_dims = {1, 3, imgSize.at(0), imgSize.at(1)};
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
                Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU), blob, 3 * imgSize.at(0) * imgSize.at(1),
                YOLO_input_node_dims.data(), YOLO_input_node_dims.size());
        auto output_tensors = mSession->Run(options, inputNodeNames.data(), &input_tensor, 1, outputNodeNames.data(),
                                            outputNodeNames.size());
        delete[] blob;
    } else {
#ifdef USE_CUDA
        half *blob = new half[iImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> YOLO_input_node_dims = {1, 3, imgSize.at(0), imgSize.at(1)};
        Ort::Value input_tensor = Ort::Value::CreateTensor<half>(
                Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU), blob, 3 * imgSize.at(0) * imgSize.at(1),
                YOLO_input_node_dims.data(), YOLO_input_node_dims.size());
        auto output_tensors = mSession->Run(options, inputNodeNames.data(), &input_tensor, 1, outputNodeNames.data(),
                                            outputNodeNames.size());
        delete[] blob;
#endif
    }
}

std::vector<TF::Detection> TF::InferenceORT::runInference(const cv::Mat &input) {
    // image
    cv::Mat frame = input;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<cv::Mat> masks;

    // Detect sub images
    std::vector<DL_RESULT> det_rets;
    RunSession(frame, det_rets);

    analysisDetResults(0, det_rets, class_ids, confidences, boxes, masks);

    //std::vector<int> nms_result;
    //cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    std::vector<Detection> detections{};
    for (unsigned long i = 0; i < class_ids.size(); ++i) {
        int idx = i;

        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.color = TFDetectManager::instance().generateClassColor(result.class_id);

        result.className = classes[result.class_id];
        result.box = boxes[idx];
        if (!masks.empty() && idx < masks.size()) {
            result.mask = masks[idx];
        }

        detections.push_back(result);
    }

    return detections;
}

void TF::InferenceORT::analysisDetResults(int x_offset,
                                            const std::vector<DL_RESULT> &detect_rets,
                                            std::vector<int> &class_ids,
                                            std::vector<float> &confidences,
                                            std::vector<cv::Rect> &boxes,
                                            std::vector<cv::Mat> &masks) {
    for (auto detect_ret: detect_rets) {
        class_ids.emplace_back(detect_ret.classId);
        confidences.emplace_back(detect_ret.confidence);

        auto x = detect_ret.box.x;
        auto y = detect_ret.box.y;
        auto w = detect_ret.box.width;
        auto h = detect_ret.box.height;
        boxes.emplace_back(x + x_offset, y, w, h);
        masks.emplace_back(detect_ret.mask);
    }
}

cv::Mat TF::InferenceORT::sigmoid(const cv::Mat &src) {
    cv::Mat dst;
    cv::exp(-src, dst);
    dst = 1.0 / (1.0 + dst);
    return dst;
}

bool TF::InferenceORT::TensorProcess(Ort::Value &inputTensor, std::vector<int64_t> &inputNodeDims,
                                       std::vector<DL_RESULT> &oResult) {
    auto outputTensor = mSession->Run(options, inputNodeNames.data(), &inputTensor, 1, outputNodeNames.data(),
                                      outputNodeNames.size());

#ifdef USE_CUDA
    auto* devicePtr = inputTensor.GetTensorMutableData<float>();
    cudaFree(devicePtr);
#endif

    Ort::TypeInfo typeInfo = outputTensor.front().GetTypeInfo();
    auto tensor_info = typeInfo.GetTensorTypeAndShapeInfo();
    std::vector<int64_t> outputNodeDims = tensor_info.GetShape();
    auto output = outputTensor.front().GetTensorMutableData<typename std::remove_pointer<float>::type>();
    int signalResultNum = outputNodeDims[1];//84
    int strideNum = outputNodeDims[2];//8400
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    std::vector<cv::Mat> masks;
    cv::Mat rawData;
    rawData = cv::Mat(signalResultNum, strideNum, CV_32F, output);
    rawData = rawData.t();
    float *data = (float *) rawData.data;

    cv::Mat protoData;
    int protoHeight = 0;
    if (outputTensor.size() > 1 && (modelType == YOLO_SEG || modelType == YOLO_SEG_HALF)) {
        auto protoInfo = outputTensor[1].GetTensorTypeAndShapeInfo();
        auto protoShape = protoInfo.GetShape();
        protoHeight = static_cast<int>(protoShape[2]);
        protoData = cv::Mat(static_cast<int>(protoShape[1]), protoHeight * static_cast<int>(protoShape[3]), CV_32F,
                            outputTensor[1].GetTensorMutableData<float>());
    }

    for (int i = 0; i < strideNum; ++i) {
        float *classesScores = data + 4;
        cv::Mat scores(1, this->classes.size(), CV_32FC1, classesScores);
        cv::Point class_id;
        double maxClassScore;
        cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);
        if (maxClassScore > rectConfidenceThreshold) {
            confidences.push_back(maxClassScore);
            class_ids.push_back(class_id.x);
            float x = data[0];
            float y = data[1];
            float w = data[2];
            float h = data[3];

            int left   = static_cast<int>((x - 0.5f * w) * mResizeScaleX);
            int top    = static_cast<int>((y - 0.5f * h) * mResizeScaleY);
            int width  = static_cast<int>(w * mResizeScaleX);
            int height = static_cast<int>(h * mResizeScaleY);

            boxes.push_back(cv::Rect(left, top, width, height));

            if (!protoData.empty()) {
                //int maskStart = 4 + static_cast<int>(this->classes.size());
                int maskStart = 4 + nc_;
                int maskDim = signalResultNum - maskStart;
                if (maskDim > 0) {
                    cv::Mat maskCoef(1, maskDim, CV_32F, data + maskStart);
                    cv::Mat mask = maskCoef * protoData;
                    mask = mask.reshape(1, protoHeight);
                    mask = sigmoid(mask);

                    cv::resize(mask, mask, cv::Size(imgSize.at(0), imgSize.at(1)));
                    cv::resize(mask, mask, mOriginalImgSize);

                    cv::Rect roi = boxes.back() & cv::Rect(0, 0, mask.cols, mask.rows);
                    cv::Mat boxMask = cv::Mat::zeros(mOriginalImgSize, CV_8UC1);
                    if (roi.width > 0 && roi.height > 0) {
                        cv::Mat maskCrop = mask(roi) > 0.5;
                        maskCrop.convertTo(maskCrop, CV_8UC1, 255.0);
                        maskCrop.copyTo(boxMask(roi));
                    }
                    masks.push_back(boxMask);
                }
            }
        }
        data += signalResultNum;
    }
    std::vector<int> nmsResult;
    cv::dnn::NMSBoxes(boxes, confidences, rectConfidenceThreshold, iouThreshold, nmsResult);
    for (int i = 0; i < nmsResult.size(); ++i) {
        int idx = nmsResult[i];
        DL_RESULT result;
        result.classId = class_ids[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        if (!masks.empty() && idx < masks.size()) {
            result.mask = masks[idx];
        }
        oResult.push_back(result);
    }
    return true;
}
