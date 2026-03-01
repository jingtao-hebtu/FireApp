/**************************************************************************

Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : FlameIRMapper.cpp
   Author : claude
   Date   : 2026/03/01
   Brief  : Implementation of FlameIRMapper
**************************************************************************/
#include "FlameIRMapper.h"
#include "TLog.h"

#include <opencv2/imgproc.hpp>
#include <cmath>
#include <fstream>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

namespace TF {
    void FlameIRMapper::init(const double H[9], int ir_width, int ir_height) {
        H_ = cv::Mat(3, 3, CV_64F);
        std::memcpy(H_.data, H, 9 * sizeof(double));
        irWidth_ = ir_width;
        irHeight_ = ir_height;
        initialized_ = true;
        LOG_F(INFO, "FlameIRMapper initialized: ir_size=%dx%d", irWidth_, irHeight_);
    }

    bool FlameIRMapper::loadFromJson(const std::string& calibPath) {
        QFile file(QString::fromStdString(calibPath));
        if (!file.open(QIODevice::ReadOnly)) {
            LOG_F(ERROR, "FlameIRMapper: cannot open %s", calibPath.c_str());
            return false;
        }

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
        file.close();

        if (parseError.error != QJsonParseError::NoError) {
            LOG_F(ERROR, "FlameIRMapper: JSON parse error: %s",
                  parseError.errorString().toStdString().c_str());
            return false;
        }

        QJsonObject root = doc.object();

        // ---- 解析H矩阵: 3×3嵌套数组 → 9个double ----
        QJsonArray hRows = root["homography_vis_to_ir"].toArray();
        if (hRows.size() != 3) {
            LOG_F(ERROR, "FlameIRMapper: H matrix must be 3x3, got %d rows",
                  static_cast<int>(hRows.size()));
            return false;
        }

        double H[9];
        for (int i = 0; i < 3; ++i) {
            QJsonArray row = hRows[i].toArray();
            if (row.size() != 3) {
                LOG_F(ERROR, "FlameIRMapper: H row %d must have 3 elements, got %d",
                      i, static_cast<int>(row.size()));
                return false;
            }
            for (int j = 0; j < 3; ++j) {
                H[i * 3 + j] = row[j].toDouble();
            }
        }

        // ---- 解析红外尺寸: "ir_image_size": [width, height] ----
        QJsonArray irSize = root["ir_image_size"].toArray();
        int w = (irSize.size() >= 1) ? irSize[0].toInt(120) : 120;
        int h = (irSize.size() >= 2) ? irSize[1].toInt(160) : 160;

        init(H, w, h);
        return true;
    }

    bool FlameIRMapper::mapBbox(const cv::Rect& visRect, cv::Rect& irRect) const {
        if (!initialized_) {
            return false;
        }

        // Construct 4 corner points of the visible bbox
        std::vector<cv::Point2f> srcPts = {
            {static_cast<float>(visRect.x), static_cast<float>(visRect.y)}, // top-left
            {static_cast<float>(visRect.x + visRect.width), static_cast<float>(visRect.y)}, // top-right
            {static_cast<float>(visRect.x + visRect.width), static_cast<float>(visRect.y + visRect.height)},
            // bottom-right
            {static_cast<float>(visRect.x), static_cast<float>(visRect.y + visRect.height)} // bottom-left
        };

        // Apply perspective transform
        std::vector<cv::Point2f> dstPts;
        cv::perspectiveTransform(srcPts, dstPts, H_);

        // Compute axis-aligned bounding rectangle of transformed points
        float xMin = dstPts[0].x, xMax = dstPts[0].x;
        float yMin = dstPts[0].y, yMax = dstPts[0].y;
        for (int i = 1; i < 4; ++i) {
            xMin = std::min(xMin, dstPts[i].x);
            xMax = std::max(xMax, dstPts[i].x);
            yMin = std::min(yMin, dstPts[i].y);
            yMax = std::max(yMax, dstPts[i].y);
        }

        int x1 = static_cast<int>(std::floor(xMin));
        int y1 = static_cast<int>(std::floor(yMin));
        int x2 = static_cast<int>(std::ceil(xMax));
        int y2 = static_cast<int>(std::ceil(yMax));

        // Clamp to IR image bounds
        x1 = std::clamp(x1, 0, irWidth_);
        y1 = std::clamp(y1, 0, irHeight_);
        x2 = std::clamp(x2, 0, irWidth_);
        y2 = std::clamp(y2, 0, irHeight_);

        int w = x2 - x1;
        int h = y2 - y1;

        if (w <= 0 || h <= 0) {
            return false;
        }

        irRect = cv::Rect(x1, y1, w, h);
        return true;
    }

    void FlameIRMapper::drawOnIR(cv::Mat& irPseudo, const cv::Rect& irRect,
                                 const cv::Scalar& color, int thickness) const {
        if (irPseudo.empty() || irRect.area() <= 0) {
            return;
        }

        cv::rectangle(irPseudo, irRect, color, thickness);
    }
} // namespace TF
