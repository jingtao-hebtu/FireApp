/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TCurveColor.h
   Author : tao.jing
   Date   : 2023/11/28
   Brief  : 
**************************************************************************/
#ifndef QONEDARK_TCURVECOLOR_H
#define QONEDARK_TCURVECOLOR_H

#include "TError.h"
#include <QColor>
#include <cstdint>
#include <map>
#include "TaoUtilDefine.h"


namespace T_QtBase {

    class TCurveColor {
    public:
        static TBase::TBaseError getColorByIndex(std::uint8_t color_idx, QColor& color);

        static TBase::TBaseError getSchemeColorByName(const std::string& scheme_name,
                                           const std::string& color_name, QColor& color);

    public:
        static const std::uint8_t ColorArrayLen = 47;
        static const std::uint8_t ColorSchemeLen = 1;

    private:
        static const QColor mCurveColorArray[ColorArrayLen];

        static const std::string mCurveColorSchemeNames[ColorSchemeLen];
        static const QColor mCurveColorSchemeDark[ColorSchemeLen];
        static const QColor mCurveColorSchemeBright[ColorSchemeLen];
        static const QColor mCurveColorSchemeGray[ColorSchemeLen];

        static std::map<std::string, void*> mCurveColorSchemeMap;

    };

};


#endif //QONEDARK_CURVECOLOR_H
