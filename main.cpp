//
//  main.cpp
//  cvAisView
//
//  Created by ZENJU Daisuke on 2015/12/07.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#include <iostream>
#include "init.hpp"
#include "geo.hpp"


// 状態遷移
int flag = 0;

// 文字描画遷移
int strflag = 0;

// Mat, マウスポイント
cv::Mat img, dst, cut;
cv::Point mousePoint;

// プロパティー読み込み
Property pro;

// 時間
int t = pro.startTime;


// マウスコールバック関数
void on_mouse(int event, int x, int y, int flags, void *param) {
    if (event == CV_EVENT_MOUSEMOVE) {
        mousePoint.x = x;
        mousePoint.y = y;
    }
}


// 時刻変換(hhmmss -> s)
int second(int time) {
    double h = floor(time / 10000);
    double m = floor((time - (h * 10000)) / 100);
    double s = time - (m * 100) - (h * 10000);
    int second = (int)(h * 3600 + m * 60 + s);
    return second;
}


// 時刻変換(s -> hhmmss)
int hour(int time) {
    double h = floor(time / 3600);
    double m = floor((time - (h * 3600)) / 60);
    double s = time - (m * 60) - (h * 3600);
    int hour = (int)(h * 10000 + m * 100 + s);
    return hour;
}


// 水平線描画(img)
void drawHorizon() {
    cv::line(img, cv::Point(0, pro.horizon)\
             , cv::Point(img.cols, pro.horizon), cv::Scalar(255, 255, 0));
}


// 画素線描画(img)
void drawImageLine() {
    for (int i = 0; i < img.cols; ++i) {
        if (i % 100 == 0) {
            cv::line(img, cv::Point(i, 0),\
                     cv::Point(i, img.rows),\
                     cv::Scalar(150, 150, 150));
        }
        if (i % 500 == 0) {
            cv::line(img, cv::Point(i, 0),\
                     cv::Point(i, img.rows),\
                     cv::Scalar(200, 200, 200));
        }
        if (i % 1000 == 0) {
            cv::line(img, cv::Point(i, 0),\
                     cv::Point(i, img.rows),\
                     cv::Scalar(250, 250, 250));
        }
    }
    for (int i = 0; i < img.rows; ++i) {
        if (i % 100 == 0) {
            cv::line(img, cv::Point(0, i),\
                     cv::Point(img.cols, i),\
                     cv::Scalar(150, 150, 150));
        }
        if (i % 500 == 0) {
            cv::line(img, cv::Point(0, i),\
                     cv::Point(img.cols, i),\
                     cv::Scalar(200, 200, 200));
        }
        if (i % 1000 == 0) {
            cv::line(img, cv::Point(0, i),\
                     cv::Point(img.cols, i),\
                     cv::Scalar(250, 250, 250));
        }
    }
}


void drawImageString() {
    if (strflag == 0) {
        std::stringstream str;
        str << std::to_string(pro.elapsedTime) << "[hhmmss]";
        cv::putText(img, str.str(), cv::Point(10,250),\
                    cv::FONT_ITALIC, 10.0,\
                    cv::Scalar(200,0,200), 8, CV_AA);
        str.str("");
        str << pro.imname;
        cv::putText(img, str.str(), cv::Point(10,500),\
                    cv::FONT_ITALIC , 10.0,\
                    cv::Scalar(200,0,200), 8, CV_AA);
    }
}


// 緯度経度計算・描画(img)
void drawShipPosition() {
    // open DB
    sqlite3 *db = NULL;
    
    int result = sqlite3_open(pro.dbname.c_str(), &db);
    if (result != SQLITE_OK) {
        std::cout << sqlite3_errmsg(db) << std::endl;
        exit(-1);
    }
    
    sqlite3_stmt *stmt = NULL;
    int qresult;
    
    // 指定時刻のmmsi, lat, lon取得
    std::string stdtime = std::to_string(pro.elapsedTime);
    std::string sql = "select mmsi, lon, lat from dec1 where time like '%"\
                    + stdtime + "'";
    std::cout << sql << std::endl;
    
    // カメラ回転角度[degree]
    double rx = pro.cameraRollX;
    double ry = pro.cameraRollY;
    double rz = pro.cameraRollZ;
    
    // enu回転座標
    cv::Mat rotx = (cv::Mat_<double>(3, 3) <<
                    1.0, 0.0, 0.0,
                    0.0, cos(rx / 180.0 * M_PI), -sin(rx / 180.0 * M_PI),
                    0.0, sin(rx / 180.0 * M_PI), cos(rx / 180.0 * M_PI)
                    );
    cv::Mat roty = (cv::Mat_<double>(3, 3) <<
                    cos(ry / 180.0 * M_PI), 0.0, sin(ry / 180.0 * M_PI),
                    0.0, 1.0, 0.0,
                    -sin(ry / 180.0 * M_PI), 0.0, cos(ry / 180.0 * M_PI)
                    );
    cv::Mat rotz = (cv::Mat_<double>(3, 3) <<
                    cos(rz / 180.0 * M_PI), -sin(rz / 180.0 * M_PI), 0.0,
                    sin(rz / 180.0 * M_PI), cos(rz / 180.0 * M_PI), 0.0,
                    0.0, 0.0, 1.0
                    );
    
    // カメラアングル制限(cameraMin[deg], cameraMax[deg])
    double cameraMin = rz - (pro.fovX / 2.0);
    double cameraMax = rz + (pro.fovX / 2.0);
    if (cameraMin >= 360.0)
        cameraMin = cameraMin - 360.0;
    if (cameraMin < 0)
        cameraMin = cameraMin + 360;
    if (cameraMax >= 360.0)
        cameraMax = cameraMax - 360.0;
    if (cameraMax < 0)
        cameraMax = cameraMax + 360;
    
    // 距離制限(distMax[m])
    double distMax = 5 * 1852;
    
    // 撮影点ECEF
    double own[3] = {0.0, 0.0, 0.0};
    geo::blh2ecef(&pro.ownLat, &pro.ownLon, &pro.ownHgt, own);
    
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    while (SQLITE_ROW == (qresult = sqlite3_step(stmt))) {
        int mmsi = sqlite3_column_int(stmt, 0);
        double lon = sqlite3_column_double(stmt, 1);
        double lat = sqlite3_column_double(stmt, 2);
        double hgt = 0.0;
        
        // 他船ECEF
        double other[3] = {0.0, 0.0, 0.0};
        geo::blh2ecef(&lat, &lon, &hgt, other);
        
        // 他船ENU
        double enu[3] = {0.0, 0.0, 0.0};
        geo::ecef2enu(own, other, enu);
        
        // 他船距離, 角度
        double dist = geo::distance(enu[0], enu[1]);
        double angle = geo::angle(enu[0], enu[1]);
        
        if (cameraMin <= angle && angle <= cameraMax && dist <= distMax) {
            //printf("\n%d: [%f,%f], d:%f, angl:%f, [%f,%f,%f]\n"\
                   ,mmsi,lat,lon, dist, angle, enu[0], enu[1], enu[2]);
            
            // enu(Mat版)
            cv::Mat position = (cv::Mat_<double>(3, 1) <<
                                enu[0], enu[1], enu[2]);
        
            // enuを回転座標で変換
            position = rotx * roty * rotz * position;
            
            // enu -> eunに変換
            cv::Mat newPosition = (cv::Mat_<double>(3, 1) <<
                                   position.at<double>(0),
                                   position.at<double>(2),
                                   position.at<double>(1));
        
            //std::cout << newPosition << std::endl;
            
            // 射影変換
            cv::Mat ans = (pro.cameraMatrix * newPosition)\
                        / position.at<double>(1);
            
            //std::cout << ans << std::endl;
            
            int imgX = int(ans.at<double>(0));
            int imgY = int(ans.at<double>(1));
            
            cv::circle(img, cv::Point(imgX, imgY),\
                       5, cv::Scalar(255,0,0), -1, CV_AA);
            
            // 船舶情報表示
            if (strflag == 0) {
                std::stringstream str;
                str << std::to_string(mmsi) << " ["\
                << std::to_string(lat) << ","\
                << std::to_string(lon) << "]";
                cv::putText(img, str.str(), cv::Point(imgX + 5,imgY),\
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,\
                            cv::Scalar(0,0,200), 1, CV_AA);
                str.str("");
                str << std::to_string(angle) << "[deg]";
                cv::putText(img, str.str(), cv::Point(imgX + 5,imgY + 20),\
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,\
                            cv::Scalar(0,0,200), 1, CV_AA);
                str.str("");
                str << std::to_string(dist) << "[m]";
                cv::putText(img, str.str(), cv::Point(imgX + 5,imgY + 40),\
                            cv::FONT_HERSHEY_COMPLEX_SMALL , 1.0,\
                            cv::Scalar(0,0,200), 1, CV_AA);

            }
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_free(db);
}


// 画像読み込み(img) & 描画処理
void readImage() {
    std::string fname = pro.imname;
    img = cv::imread(fname);
    
    // 画像上描画処理
    drawImageLine();
    drawImageString();
    drawHorizon();
    drawShipPosition();
    
    flag = 1;
}


// 画像リサイズ(img->dst) & リサイズ画像表示(dst)
void showResizeImage() {
    cv::resize(img, dst, cv::Size(), pro.imresize, pro.imresize);
    cv::imshow("resize image", dst);
    cvSetMouseCallback("resize image", on_mouse);
    flag = 2;
}


// 画像切り取り(img->cut) & 切り取り画像表示(cut)
void showCutImage() {
    int xl = pro.imcutWidth;
    int yl = pro.imcutHeight;
    int x0 = (int)((mousePoint.x / pro.imresize) - (xl / 2));
    int y0 = (int)((mousePoint.y / pro.imresize) - (yl / 2));
    
    
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if ((xl + x0) > img.cols) x0 = img.cols - xl;
    if ((yl + y0) > img.rows) y0 = img.rows - yl;
    
    cut = cv::Mat(img, cv::Rect(x0, y0, xl, yl));
    cv::imshow("cut image", cut);
}


// 画像ファイル名, 時刻変更
void changeImage() {
    int num;
    std::cin >> num;
    pro.imnumber(num);
    
    int stime = pro.startTime;
    int sec = second(stime) + ((num - 1) * pro.interval);
    int time = hour(sec);
    pro.changeElapsedTime(time);
    flag = 0;
}


// キーボード入力に応じた処理
void waitImage() {
    int c = cv::waitKey();
    
    switch (c) {
        case 27:
            // ESC(27): quit
            flag = 3;
            std::cout << "Quit" << std::endl;
            break;
            
        case 32:
            // SPACE(32): cut image
            showCutImage();
            break;
            
        case 13:
            // ENTER(13): read image number
            std::cout << "input image number: ";
            changeImage();
            break;
            
        case 115:
            // S(115): draw ship information
            if (strflag == 0)
                strflag = 1;
            else
                strflag = 0;
            flag = 0;
            break;
            
        default:
            //std::cout << c << std::endl;
            break;
    }
}


// シーケンス処理
void windowImage() {
    while (true) {
        switch (flag) {
            case 0:
                //read image
                readImage();
                std::cout << "read file: " << pro.imname << std::endl;
                continue;
                
            case 1:
                //show resize image(resize)
                showResizeImage();
                std::cout << "show resize image:" << pro.imname << std::endl;
                continue;
                
            case 2:
                // wait key & action
                waitImage();
                continue;
                
            case 3:
                // exit program
                exit(0);
                break;
                
            default:
                break;
        }
    }
}


// main
int main(int argc, char * argv[]) {
    pro.readCameraParameter();
    windowImage();
    
    return 0;
}