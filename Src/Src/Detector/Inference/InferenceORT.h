/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TbInferenceORT.h
   Author : tao.jing
   Date   : 2024/11/17
   Brief  :
**************************************************************************/
#ifndef EPDETECTION_TBINFERENCEORT_H
#define EPDETECTION_TBINFERENCEORT_H


#include <vector>
#include <string>
#include "../DetectDef.h"
#include <opencv2/opencv.hpp>
#include "onnxruntime_cxx_api.h"
//#include "TbDetectManager.h"


#ifdef USE_CUDA

#include <cuda_fp16.h>

#endif


namespace TF {

    enum MODEL_TYPE {
        //FLOAT32 MODEL
        YOLO_DETECT = 1,
        YOLO_POSE = 2,
        YOLO_CLS = 3,
        YOLO_SEG = 4,

        //FLOAT16 MODEL
        YOLO_DETECT_HALF = 5,
        YOLO_POSE_HALF = 6,
        YOLO_CLS_HALF = 7,
        YOLO_SEG_HALF = 8
    };

    typedef struct _DL_INIT_PARAM {
        std::string modelPath;
        MODEL_TYPE modelType = YOLO_DETECT;
        std::vector<int> imgSize = {TF_DETECT_IMG_SIZE, TF_DETECT_IMG_SIZE};
        float rectConfidenceThreshold = 0.6f;
        float iouThreshold = 0.5f;
        int keyPointsNum = 2;
        bool cudaEnable = false;
        int logSeverityLevel = 3;
        int intraOpNumThreads = 1;
    } DL_INIT_PARAM;

    typedef struct _DL_RESULT {
        int classId;
        float confidence;
        cv::Rect box;
        std::vector<cv::Point2f> keyPoints;
        cv::Mat mask;
    } DL_RESULT;

    class InferenceORT {

    public:
        InferenceORT(const std::string &model_path);

        ~InferenceORT();

    public:
        std::vector<Detection> runInference(const cv::Mat &input);

        bool CreateSession(DL_INIT_PARAM &iParams);

        void RunSession(cv::Mat &iImg, std::vector<DL_RESULT> &oResult);

        void WarmUpSession();

        template<typename N>
        bool TensorProcess(N &blob,
                           std::vector<int64_t> &inputNodeDims,
                           std::vector<DL_RESULT> &oResult);

        bool TensorProcess(Ort::Value& inputTensor,
                           std::vector<int64_t> &inputNodeDims,
                           std::vector<DL_RESULT> &oResult);

        void PreProcess(cv::Mat &iImg, std::vector<int> iImgSize, cv::Mat &oImg);

    private:
        void analysisDetResults(int x_offset,
                                const std::vector<DL_RESULT> &detect_rets,
                                std::vector<int>& class_ids,
                                std::vector<float>& confidences,
                                std::vector<cv::Rect>& boxes,
                                std::vector<cv::Mat>& masks);

        cv::Mat sigmoid(const cv::Mat& src);

    public:
        std::vector<std::string> classes{
                "introduction_device",
                "introduction_device_not_sealed",
                "introduction_device_multiple_entries",
                "fastening_device",
                "fastening_loose",
                "fastening_device_missing",
                "ground_wire_label",
                "ground_wire",
                "tar_box"
        };

    private:
        Ort::Env mEnv;
        Ort::Session *mSession;
        bool cudaEnable;
        Ort::RunOptions options;
        std::vector<const char *> inputNodeNames;
        std::vector<const char *> outputNodeNames;

        MODEL_TYPE modelType;
        std::vector<int> imgSize;
        float rectConfidenceThreshold;
        float iouThreshold;
        float mResizeScales {1080.0f / 800.0f};
        cv::Size mOriginalImgSize{};

        float modelScoreThreshold{0.45f};
        float modelNMSThreshold{0.50f};
    };

};


#endif //EPDETECTION_TBINFERENCEORT_H
