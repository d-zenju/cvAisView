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
#include <opencv2/opencv.hpp>

class Property {
private:
    // 画像ファイル名(ディレクトリー, 拡張子, 連番)
    int imnum_min = 1, imnum_max = 443;
    std::string directory = "/Volumes/HDPC-UT/ships/TIF/ST_";
    std::string imnum = std::to_string(imnum_min);
    std::string ext = ".TIF";
    
public:
    // 画像ファイル名
    std::string imname = directory + imnum + ext;
    
    // 画像リサイズ率
    double imresize = 0.1;
    
    // 画像切り取りサイズ
    int imcutWidth = 1000;
    int imcutHeight = 600;
    
    // 水平線位置
    int horizon = 2418;
    
    // AISデータベース名
    std::string dbname = "/Users/zenju/Desktop/aivdm.sqlite3";
    
    // 撮影時刻(UTC, hhmmss), 撮影時間感覚(sec)
    int startTime = 12100;
    int finishTime = 50130;
    int interval = 30;
    
    // 撮影地点座標(緯度,経度,高さ,方角)
    double ownLat = 35.311755;
    double ownLon = 139.785922;
    double ownHgt = 23.0;
    double ownDir = 180.0;
    
    // カメラパラメータ(焦点距離, イメージセンサーサイズ)
    double cameraFocalLength = 28 * pow(10, -3);
    double cameraSensorX = 35.9 * pow(10, -3);
    double cameraSensorY = 24.0 * pow(10, -3);
    
    
    // method: 画像ファイル名変更
    void imnumber(int num);
};

#endif /* init_hpp */
