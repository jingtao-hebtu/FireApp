/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : Detector.cpp
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#include "Detector.h"
#include "InferenceORT.h"
#include "InferenceTRT.h"
#include "TSysUtils.h"
#include "TFException.h"
#include "TConfig.h"
#include "TLog.h"

#include <opencv2/opencv.hpp>


namespace TF {
    static cv::Scalar GetColorByClassId(int class_id) {
        static const cv::Scalar palette[] = {
            {255, 56, 56}, {255, 157, 151}, {255, 112, 31}, {255, 178, 29},
            {207, 210, 49}, {72, 249, 10}, {146, 204, 23}, {61, 219, 134},
            {26, 147, 52}, {0, 212, 187}, {44, 153, 168}, {0, 194, 255},
            {52, 69, 147}, {100, 115, 255}, {0, 24, 236}, {132, 56, 255},
            {82, 0, 133}, {203, 56, 255}, {255, 149, 200}, {255, 55, 199}
        };
        constexpr int N = sizeof(palette) / sizeof(palette[0]);
        if (class_id < 0) class_id = 0;
        return palette[class_id % N];
    }

    static std::vector<Detection> SegmentResToDetections(
        const trtyolo::SegmentRes& result,
        const cv::Size& origSize, // 原图大小：input_frame.size()
        const cv::Size& inferSize, // 推理大小：cv::Size(640, 640)
        const std::vector<std::string>& labels,
        float mask_thresh = 0.5f
    ) {
        std::vector<Detection> detections;

        const int orig_w = origSize.width;
        const int orig_h = origSize.height;

        // 640 -> 原图 的非等比缩放系数（因为你用的是 cv::resize 拉伸）
        const float sx = (float)orig_w / (float)inferSize.width;
        const float sy = (float)orig_h / (float)inferSize.height;

        // 防御性：以 num 与各数组实际长度的最小值为准
        int n = result.num;
        n = std::min(n, (int)result.boxes.size());
        n = std::min(n, (int)result.classes.size());
        n = std::min(n, (int)result.scores.size());
        n = std::min(n, (int)result.masks.size());

        detections.reserve(n);

        for (int i = 0; i < n; ++i) {
            const auto& box = result.boxes[i];
            int cls = result.classes[i];
            float score = result.scores[i];

            // -------- 1) bbox：从 640 空间映射回原图空间 --------
            // 注意：box.left/top/right/bottom 在官方 visualize 里就是“像素坐标”
            // 这里只做拉伸的反变换：x*sx, y*sy
            float l = box.left * sx;
            float t = box.top * sy;
            float r = box.right * sx;
            float b = box.bottom * sy;

            // 处理可能的左右/上下颠倒
            float x_min_f = std::min(l, r);
            float x_max_f = std::max(l, r);
            float y_min_f = std::min(t, b);
            float y_max_f = std::max(t, b);

            // 这里的 xyxy 用 int，保持和官方逻辑一致（后面要用 +1 计算宽高）
            int x0 = (int)std::floor(x_min_f);
            int y0 = (int)std::floor(y_min_f);
            int x1 = (int)std::floor(x_max_f);
            int y1 = (int)std::floor(y_max_f);

            int w = std::max(x1 - x0 + 1, 1);
            int h = std::max(y1 - y0 + 1, 1);

            // 在原图上的可用区域（裁剪后）
            int X1 = std::max(0, x0);
            int Y1 = std::max(0, y0);
            int X2 = std::min(orig_w, x1 + 1); // exclusive
            int Y2 = std::min(orig_h, y1 + 1); // exclusive

            int target_w = X2 - X1;
            int target_h = Y2 - Y1;
            if (target_w <= 0 || target_h <= 0) {
                continue;
            }

            Detection det;
            det.class_id = cls;
            det.confidence = score;
            det.color = GetColorByClassId(cls);
            if (cls >= 0 && cls < (int)labels.size()) det.className = labels[cls];
            else det.className = "cls_" + std::to_string(cls);

            det.box = cv::Rect(X1, Y1, target_w, target_h);

            // -------- 2) mask：严格参考官方逻辑（resize到bbox，再贴回bbox）--------
            const auto& mk = result.masks[i];
            if (!mk.data.empty() && mk.width > 0 && mk.height > 0) {
                // 模型输出浮点 mask
                cv::Mat float_mask(mk.height, mk.width, CV_32FC1, (void*)mk.data.data());
                // 注意：不在原地 resize，避免 Mat 视图引用的混乱
                cv::Mat resized_mask;
                cv::resize(float_mask, resized_mask, cv::Size(w, h), 0, 0, cv::INTER_LINEAR);

                // 二值化 -> CV_8U（这一点你给的“官方代码”片段少了 convert，会导致类型不匹配）
                cv::Mat bool_mask; // CV_8U, 0/255
                cv::compare(resized_mask, mask_thresh, bool_mask, cv::CMP_GT);

                // 全图 mask（原图大小）
                cv::Mat mask_image = cv::Mat::zeros(orig_h, orig_w, CV_8UC1);

                // 若 bbox 左上角为负，源 mask 需要裁切偏移
                int src_x_offset = std::max(0, -x0);
                int src_y_offset = std::max(0, -y0);

                // 目标区域尺寸（已裁剪到图像内）
                int tw = target_w;
                int th = target_h;

                // 防止 source_rect 超出 bool_mask
                if (src_x_offset + tw > bool_mask.cols) tw = bool_mask.cols - src_x_offset;
                if (src_y_offset + th > bool_mask.rows) th = bool_mask.rows - src_y_offset;
                if (tw <= 0 || th <= 0) {
                    // bbox 在图里，但对应 mask 裁剪后没剩下有效区域
                    det.mask.release();
                }
                else {
                    cv::Rect target_rect(X1, Y1, tw, th);
                    cv::Rect source_rect(src_x_offset, src_y_offset, tw, th);

                    bool_mask(source_rect).copyTo(mask_image(target_rect));
                    det.mask = std::move(mask_image);
                }
            }
            detections.emplace_back(std::move(det));
        }
        return detections;
    }


    Detector::Detector() {
    }

    bool Detector::init() {
        bool ret = false;
        mDetMode = GET_STR_CONFIG("VisionMea", "DetMode");
        if (mDetMode == "ORT") {
            ret = initORT();
        }
        else if (mDetMode == "TRT") {
            ret = initTRT();
        }
        else {
            TF_LOG_THROW_RUNTIME("[Detector::init] Invalid DetMode %s.", mDetMode.c_str());
        }
        return ret;
    }

    bool Detector::initORT() {
        auto model_path = GET_STR_CONFIG("VisionMea", "DetModelName");
        LOG_F(INFO, "Load detection model %s.", model_path.c_str());

        if (!TBase::fileExists(model_path)) {
            LOG_F(ERROR, "Model file %s not exists.", model_path.c_str());
        }

        try {
            mInfORT = new InferenceORT(model_path);
            LOG_F(INFO, "[TbDetector] TbInferenceORT model loaded.");
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, %s.", ex.what());
        }
        catch (const std::string& ex) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, %s.", ex.c_str());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] TbInference model load failed, unknown exception.");
        }
        return false;
    }

    bool Detector::initTRT() {
        try {
            mInfTRT = new InferenceTRT();
            LOG_F(INFO, "[TbDetector] InferenceTRT model loaded.");
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] InferenceTRT model load failed, %s.", ex.what());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] InferenceTRT model load failed, unknown exception.");
        }
        return false;
    }

    bool Detector::runDetect(const std::string& im_path, size_t& detect_num, std::vector<Detection>& detections,
                             cv::Mat& frame) {
        if (!TBase::fileExists(im_path)) {
            LOG_F(ERROR, "Image path %s not exists.", im_path.c_str());
            return false;
        }

        if (mInfORT == nullptr) {
            LOG_F(ERROR, "TbInference not init.");
            return false;
        }

        try {
            frame = cv::imread(im_path);

            detections = mInfORT->runInference(frame);
            detect_num = detections.size();

            for (auto idx = 0; idx < detect_num; idx++) {
                const auto& detection = detections[idx];
                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                if (!detection.mask.empty()) {
                    cv::Mat maskColor(frame.size(), frame.type(), color);
                    maskColor.copyTo(frame, detection.mask);
                }
                cv::rectangle(frame, box, color, 2);

                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
                cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

                cv::rectangle(frame, textBox, color, cv::FILLED);
                cv::putText(frame, classString,
                            cv::Point(box.x + 5, box.y - 10),
                            cv::FONT_HERSHEY_DUPLEX, 1,
                            cv::Scalar(0, 0, 0), 2, 0);
            }
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.what());
        }
        catch (const std::string& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.c_str());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] Run inference failed");
        }
        return false;
    }

    bool Detector::runDetect(const cv::Mat& input_frame,
                             size_t& detect_num,
                             std::vector<Detection>& detections) {
        if (input_frame.empty()) {
            return false;
        }

        detections = mInfORT->runInference(input_frame);
        detect_num = detections.size();
        return true;
    }

    bool Detector::runDetectWithPreview(cv::Mat& input_frame,
                                        size_t& detect_num,
                                        std::vector<Detection>& detections) {
        if (input_frame.empty()) {
            return false;
        }

        try {
            if (mDetMode == "TRT") {
                std::vector<std::string> class_names;
                class_names.emplace_back("fire");
                auto result = mInfTRT->runInference(input_frame);

                const cv::Size inferSize(640, 640);
                detections = SegmentResToDetections(result, input_frame.size(), inferSize, class_names, 0.5f);
            }

            if (mDetMode == "ORT") {
                detections = mInfORT->runInference(input_frame);
            }

            detect_num = detections.size();

            for (auto idx = 0; idx < detect_num; idx++) {
                const auto& detection = detections[idx];

                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                if (!detection.mask.empty()) {
                    cv::Mat maskColor(input_frame.size(), input_frame.type(), color);
                    maskColor.copyTo(input_frame, detection.mask);
                }
                cv::rectangle(input_frame, box, color, 4);
                /*
                std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
                cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
                cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

                cv::rectangle(input_frame, textBox, color, cv::FILLED);
                cv::putText(input_frame, classString,
                            cv::Point(box.x + 5, box.y - 10),
                            cv::FONT_HERSHEY_DUPLEX, 1,
                            cv::Scalar(0, 0, 0), 2, 0);*/
            }
            return true;
        }
        catch (const std::exception& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.what());
        }
        catch (const std::string& ex) {
            LOG_F(ERROR, "[TbDetector] Run inference failed, %s.", ex.c_str());
        }
        catch (...) {
            LOG_F(ERROR, "[TbDetector] Run inference failed");
        }
        return false;
    }
};
