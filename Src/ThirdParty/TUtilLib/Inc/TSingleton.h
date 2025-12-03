/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TSingleton.h
   Author : tao.jing
   Date   : 2023/11/11
   Brief  : 
**************************************************************************/

#ifndef BREAKERMEA_TSINGLETON_H
#define BREAKERMEA_TSINGLETON_H


namespace TBase {


    template<class T>
    class TSingleton {
    public:
        typedef T type;

    protected:
        TSingleton();

    public:
        TSingleton(const TSingleton &) = delete;

        TSingleton &operator=(const TSingleton &) = delete;

    public:
        virtual ~TSingleton() {
        }

    public:
        static T &instance();

        static T *instancePTR();
    }; // class Singleton<T>

    // template implementation
    template<class T>
    TSingleton<T>::TSingleton() {
    }

    template<class T>
    T &
    TSingleton<T>::instance() {
        static T singleInstance;
        return singleInstance;
    }

    template<class T>
    T *
    TSingleton<T>::instancePTR() {
        return &instance();
    }

}

#endif //BREAKERMEA_TSINGLETON_H
