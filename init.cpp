//
//  init.cpp
//  cvAisView
//
//  Created by ZENJU Daisuke on 2015/12/07.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#include "init.hpp"

void Property::imnumber(int num) {
    if (imnum_min <= num && num <= imnum_max) {
        imnum = std::to_string(num);
        imChangeName();
    }
}

void Property::imChangeName(void) {
    imname = directory + imnum + ext;
}

void Property::readCameraParameter(void) {
    aspectRatio = 0.9969253427270188;
    focalLength = 28.640349690275322;
    fovX = 64.15388380487985;
    fovY = 45.522883187656035;
    retval = 2.907776055516048;
    principalPoint.x = 18.167827705173973;
    principalPoint.y = 13.206977911034718;
    cameraMatrix = (cv::Mat_<double>(3, 3) << 5871.670577170651, 0.0, 3724.6577133727146, 0.0, 5853.617202526004, 2703.028145791772, 0.0, 0.0, 1.0);
    distCoeffs = (cv::Mat_<double>(1, 5) << -0.11491412165273378, 0.04172825192513017, 0.005586375965910073, -0.0006344566526961314, 0.020941454221265008);
}

void Property::changeElapsedTime(int time) {
    elapsedTime = time;
}