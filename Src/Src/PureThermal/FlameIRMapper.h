/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FlameIRMapper.h
   Author : claude
   Date   : 2026/03/01
   Brief  : Maps visible-light flame bboxes to infrared image coordinates
            using a pre-calibrated homography matrix H.
**************************************************************************/
#ifndef FIREAPP_FLAMEIRMAP_H
#define FIREAPP_FLAMEIRMAP_H

#include <opencv2/core.hpp>
#include <string>

namespace TF {

    class FlameIRMapper {
    public:
        /// Initialize with a 3x3 homography (row-major double[9]) and IR image size (after rotation).
        void init(const double H[9], int ir_width, int ir_height);

        /// Load H matrix and IR size from a calibration JSON file.
        /// Expected keys: "H" (array of 9 doubles), "ir_width", "ir_height".
        bool loadFromJson(const std::string& calibPath);

        /// Map a visible-light bbox to the corresponding infrared bbox.
        /// Returns true if the mapped bbox has non-zero area within the IR image bounds.
        bool mapBbox(const cv::Rect& visRect, cv::Rect& irRect) const;

        /// Draw the mapped IR bbox on a pseudocolor image.
        void drawOnIR(cv::Mat& irPseudo, const cv::Rect& irRect,
                      const cv::Scalar& color = cv::Scalar(0, 0, 255),
                      int thickness = 1) const;

        [[nodiscard]] bool isReady() const { return initialized_; }

    private:
        cv::Mat H_;            // 3x3 CV_64F
        int irWidth_  = 120;
        int irHeight_ = 160;
        bool initialized_ = false;
    };

} // namespace TF

#endif // FIREAPP_FLAMEIRMAP_H
