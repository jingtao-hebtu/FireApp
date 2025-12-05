/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TCurveColor.cpp
   Author : tao.jing
   Date   : 2023/11/28
   Brief  :
**************************************************************************/
#include "TCurveColor.h"


namespace T_QtBase {

    TBase::TBaseError TCurveColor::getColorByIndex(std::uint8_t color_idx, QColor& color) {
        std::uint8_t valid_color_idx = color_idx % ColorArrayLen;
        color = mCurveColorArray[valid_color_idx];
        return TBase::TBaseError::E_T_SUCCESS;
    }

    TBase::TBaseError TCurveColor::getSchemeColorByName(const std::string &scheme_name,
                                                 const std::string &color_name,
                                                 QColor &color) {

        if (mCurveColorSchemeMap.find(scheme_name) == mCurveColorSchemeMap.end()) {
            return TBase::E_T_CURVE_COLOR_INVALID_KEY;
        }

        std::uint8_t color_name_idx = 0;
        bool found = false;
        for (auto idx = 0; idx < ColorSchemeLen; idx++) {
            if (color_name == mCurveColorSchemeNames[idx]) {
                color_name_idx = idx;
                found = true;
                break;
            }
        }

        if (!found) {
            return TBase::E_T_CURVE_COLOR_INVALID_KEY;
        }

        auto q_scheme_color_map = reinterpret_cast<QColor*>(mCurveColorSchemeMap[scheme_name]);
        color = q_scheme_color_map[color_name_idx];
        return TBase::E_T_SUCCESS;
    }

    const QColor TCurveColor::mCurveColorArray[ColorArrayLen] = {
            QColor(192, 64, 64, 255),
            QColor(192, 136, 64, 255),
            QColor(176, 192, 64, 255),
            QColor(104, 192, 64, 255),
            QColor(64, 192, 96, 255),
            QColor(64, 192, 168, 255),
            QColor(64, 144, 192, 255),
            QColor(64, 72, 192, 255),
            QColor(128, 64, 192, 255),
            QColor(192, 64, 184, 255),
            QColor(192, 64, 112, 255),
            QColor(192, 88, 64, 255),
            QColor(192, 160, 64, 255),
            QColor(152, 192, 64, 255),
            QColor(80, 192, 64, 255),
            QColor(64, 192, 120, 255),
            QColor(64, 192, 192, 255),
            QColor(64, 120, 192, 255),
            QColor(80, 64, 192, 255),
            QColor(152, 64, 192, 255),
            QColor(192, 64, 160, 255),
            QColor(192, 64, 88, 255),
            QColor(192, 112, 64, 255),
            QColor(192, 184, 64, 255),
            QColor(128, 192, 64, 255),
            QColor(64, 192, 72, 255),
            QColor(64, 192, 144, 255),
            QColor(64, 168, 192, 255),
            QColor(64, 96, 192, 255),
            QColor(104, 64, 192, 255),
            QColor(176, 64, 192, 255),
            QColor(192, 64, 64, 255),
            QColor(192, 64, 64, 255),
            QColor(192, 136, 64, 255),
            QColor(176, 192, 64, 255),
            QColor(104, 192, 64, 255),
            QColor(64, 192, 96, 255),
            QColor(64, 192, 168, 255),
            QColor(64, 144, 192, 255),
            QColor(64, 72, 192, 255),
            QColor(128, 64, 192, 255),
            QColor(192, 64, 184, 255),
            QColor(192, 64, 112, 255),
            QColor(192, 88, 64, 255),
            QColor(192, 160, 64, 255),
            QColor(152, 192, 64, 255),
            QColor(80, 192, 64, 255),
    };

    const std::string TCurveColor::mCurveColorSchemeNames[ColorSchemeLen] = {
            "CurveViewBkColor"
    };

    const QColor TCurveColor::mCurveColorSchemeDark[ColorSchemeLen] = {
            QColor(0, 0, 0),
    };

    const QColor TCurveColor::mCurveColorSchemeBright[ColorSchemeLen] = {
            QColor(0xFFFFFF),
    };

    const QColor TCurveColor::mCurveColorSchemeGray[ColorSchemeLen] = {
            QColor(0x272c36),
    };

    std::map<std::string, void*> TCurveColor::mCurveColorSchemeMap {
            {std::string("Dark"), reinterpret_cast<void*>(const_cast<QColor*>(mCurveColorSchemeDark))},
            {std::string("Bright"), reinterpret_cast<void*>(const_cast<QColor*>(mCurveColorSchemeBright))},
            {std::string("Gray"), reinterpret_cast<void*>(const_cast<QColor*>(mCurveColorSchemeGray))}
    };


};

