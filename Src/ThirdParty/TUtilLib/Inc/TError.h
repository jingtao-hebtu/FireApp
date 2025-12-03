/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TError.h
   Author : tao.jing
   Date   : 2023/11/27
   Brief  : 
**************************************************************************/

#ifndef BREAKERMEA_TERROR_H
#define BREAKERMEA_TERROR_H

namespace TBase {


    enum TBaseError {
        E_T_SUCCESS = 0,

        E_T_YAML_KEY_NOT_EXISTS,
        E_T_YAML_NODE_NOT_SCALAR,
        E_T_YAML_NODE_NOT_SEQ,

        E_T_CURVE_COLOR_INVALID_KEY,
    };


};


#endif //BREAKERMEA_TERROR_H
