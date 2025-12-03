/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TMacros.h
   Author : tao.jing
   Date   : 2023/11/25
   Brief  : 
**************************************************************************/
#ifndef BREAKERMEA_TMACROS_H
#define BREAKERMEA_TMACROS_H


#ifdef TAO_UNUSED
#undef TAO_UNUSED
#endif
#define TAO_UNUSED(expr) do { (void)(expr); } while (0)


#define TAO_MAX_PATH_LEN 256

#ifdef TAO_DOUBLE_EQUAL
#undef TAO_DOUBLE_EQUAL
#endif
#define TAO_DOUBLE_EQUAL(x, y) (((x) - (y)) < 1E-6 && ((x) - (y)) > -1E-6)


#endif //BREAKERMEA_TMACROS_H
