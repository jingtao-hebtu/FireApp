/**************************************************************************

           Copyright(C), tao.jing All rights reserved

 **************************************************************************
   File   : TFDetectTest.cpp
   Author : tao.jing
   Date   : 2024/11/2
   Brief  :
**************************************************************************/
#include "TFDetectTest.h"
#include "DetectManager.h"
#include "opencv2/opencv.hpp"
#include <iostream>


void detectTest() {
    TF::TFDetectManager::instance().init();

    //std::string im_path("E:\\Project\\CNOOC\\EP\\Data\\Image\\EP1.png");
    std::string im_path("E:\\Project\\Fire\\FireDet\\Data\\Video\\2.jpg");
    //std::string im_path("D:\\Code\\05_DeepL\\Ultralytics\\ultralytics-241102\\examples\\YOLOv8-CPP-Inference\\images\\test.jpg");

    std::size_t detect_num;
    std::vector<TF::Detection> detections;
    cv::Mat frame = cv::imread(im_path);
    //cv::resize(frame, frame, cv::Size(800, 800));

    TF::TFDetectManager::instance().runDetectWithSlice(frame, detect_num, detections);
    //Tb::TbDetectManager::instance().runDetect(frame, detect_num, detections);
    std::cout << detect_num << std::endl;

    for (int i = 0; i < detect_num; ++i) {
        const auto& detection = detections[i];

        cv::Rect box = detection.box;
        cv::Scalar color = detection.color;

        // Detection box
        cv::rectangle(frame, box, color, 2);

        // Detection box text
        std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
        cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

        cv::rectangle(frame, textBox, color, cv::FILLED);
        cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
    }

    float scale = 0.6f;
    cv::resize(frame, frame,
               cv::Size(static_cast<int>(frame.cols*scale), static_cast<int>(frame.rows*scale)));
    cv::imshow("Inference", frame);

    cv::waitKey(-1);
}
