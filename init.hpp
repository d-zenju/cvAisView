//
//  init.hpp
//  cvAisView
//
//  Created by ZENJU Daisuke on 2015/12/07.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#ifndef init_hpp
#define init_hpp

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <sqlite3.h>


class Property {
private:
    // 画像ファイル名(ディレクトリー, 拡張子, 連番)
    int imnum_min = 1, imnum_max = 443;
    std::string directory = "/Volumes/HDPC-UT/ships/TIF/ud_ST_";
    std::string imnum = std::to_string(imnum_min);
    std::string ext = ".TIF";
    
public:
    // method: 画像ファイル番号変更
    void imnumber(int num);
    
    // method: 画像ファイル名変更
    void imChangeName(void);
    
    // method: カメラパラメータ読込
    void readCameraParameter(void);
    
    // method: 経過時間変更
    void changeElapsedTime(int time);
    
    
    // 画像ファイル名
    std::string imname = directory + imnum + ext;
    
    // 画像リサイズ率
    double imresize = 0.1;
    
    // 画像切り取りサイズ
    int imcutWidth = 1000;
    int imcutHeight = 600;
    
    // 水平線位置
    int horizon = 2430;
    
    // AISデータベース名
    std::string dbname = "/Users/zenju/Desktop/aivdm/aivdm.sqlite3";
    
    // 撮影時刻(UTC, hhmmss), 撮影時間間隔(sec), 初期経過時間(sec), 経過時間(sec)
    int startTime = 12100;
    int finishTime = 50130;
    int interval = 30;
    int firstTime = 4830;
    int elapsedTime = firstTime;
    
    // 撮影地点座標(緯度,経度,高さ)
    //double ownLat = 35.311755;
    //double ownLon = 139.785922;
    //double ownHgt = 23.0;
    double ownLat = 35.32136389;
    double ownLon = 139.78674742;
    double ownHgt = 23.8;
    
    // カメラパラメータ(焦点距離, イメージセンサーサイズ)
    double cameraFocalLength = 28 * pow(10, -3);
    double cameraSensorX = 35.9 * pow(10, -3);
    double cameraSensorY = 24.0 * pow(10, -3);
    
    // カメラパラメータ(カメラ回転)
    double cameraRollX = -2.5;
    double cameraRollY = 0.0;
    double cameraRollZ = 188.0;
    
    // カメラパラメータ(キャリブレーション値)
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Point principalPoint;
    double aspectRatio;
    double focalLength;
    double retval;
    double fovX;
    double fovY;
};

#endif /* init_hpp */
