//
//  main.cpp
//  cvAisView
//
//  Created by ZENJU Daisuke on 2015/12/07.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#include <iostream>
#include "init.hpp"


// 状態遷移
int flag = 0;

// Mat, マウスポイント
cv::Mat img, dst, cut;
cv::Point mousePoint;

// プロパティー読み込み
Property pro;


// マウスコールバック関数
void on_mouse(int event, int x, int y, int flags, void *param) {
    if (event == CV_EVENT_MOUSEMOVE) {
        mousePoint.x = x;
        mousePoint.y = y;
    }
}


// 水平線描画(img)
void drawHorizon() {
    cv::line(img, cv::Point(0, pro.horizon)\
             , cv::Point(img.cols, pro.horizon), cv::Scalar(255, 255, 0));
}


// 画像読み込み(img) & 描画処理
void readImage() {
    std::string fname = pro.imname;
    img = cv::imread(fname);
    
    // 画像上描画処理
    drawHorizon();
    
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


// 画像ファイル名変更
void changeImage() {
    int num;
    std::string oldname = pro.imname;
    
    std::cin >> num;
    pro.imnumber(num);
    
    if (oldname == pro.imname)
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
    windowImage();
    
    return 0;
}