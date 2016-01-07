//
//  geo.cpp
//  geo
//
//  Created by ZENJU Daisuke on 2015/12/24.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#include "geo.hpp"


// 楕円体高(標高+ジオイド高)
double geo::height(double altitude, double geoid) {
    return altitude + geoid;
}


// NNの計算
double geo::NN(double p) {
    return (A / sqrt(1.0 - (E2)\
                     * (sin(p * M_PI / 180.0) * (sin(p * M_PI / 180.0) ))));
}


// 緯度経度高度をxに変換する
double geo::blh2x(double latitude, double longitude, double height) {
    return (NN(latitude)+height)\
    * cos(latitude * M_PI / 180.0) * cos(longitude * M_PI / 180.0);
}


// 緯度経度高度をyに変換する
double geo::blh2y(double latitude, double longitude, double height) {
    return (NN(latitude)+height)\
    * cos(latitude * M_PI / 180.0) * sin(longitude * M_PI / 180.0);
}


// 緯度経度高度をzに変換する
double geo::blh2z(double latitude, double longitude, double height) {
    return (NN(latitude) * (1 - E2) + height) * sin(latitude * M_PI / 180.0);
}


// 緯度軽度高度からECEFに変換する
void geo::blh2ecef(double *latitude, double *longitude, double *height, double *ecef) {
    ecef[0] = blh2x(*latitude, *longitude, *height);
    ecef[1] = blh2y(*latitude, *longitude, *height);
    ecef[2] = blh2z(*latitude, *longitude, *height);
}


// ECEF(x, y, z)から緯度(φ)に変換する
double geo::ecef2lat(double x, double y, double z) {
    double p = sqrt(x * x + y * y);
    double theta = atan2(z * A, p * B);
    double ans = (180.0 / M_PI) * atan2((z + ED2 * B * pow(sin(theta), 3)),\
                                      (p - E2 * A * pow(cos(theta), 3)));
    
    return ans;
}


// ECEF(x, y, z)から経度(Λ)に変換する
double geo::ecef2lon(double x, double y) {
    return (180.0 / M_PI) * atan2(y, x);
}


// ECEF(x, y, z)から高度(h)に変換する
double geo::ecef2hgt(double x, double y, double z) {
    double phi = geo::ecef2lat(x, y, z);
    double p = sqrt(x * x + y * y);
    double ans = (p / cos(phi * M_PI / 180.0)) - NN(phi);
    return ans;
}


// ECEF(x, y, z) to ENU(x', y', z')
void geo::ecef2enu(double *own, double *other, double *enu) {
    double p[3] = {0.0, 0.0, 0.0};
    p[0] = other[0] - own[0];
    p[1] = other[1] - own[1];
    p[2] = other[2] - own[2];
    
    double rot0[3][3] = {0.0, 0.0, 0.0};
    double rot1[3][3] = {0.0, 0.0, 0.0};
    double rot2[3][3] = {0.0, 0.0, 0.0};
    
    Rz(90.0 * M_PI / 180.0, rot0);
    Ry((90.0 - ecef2lat(own[0], own[1], own[2]))\
            * M_PI / 180.0, rot1);
    Rz(ecef2lon(own[0], own[1]) * M_PI / 180.0, rot2);
    
    double m[3][3] = {0.0, 0.0, 0.0};
    double r[3][3] = {0.0, 0.0, 0.0};
    MM(rot0, rot1, m);
    MM(m, rot2, r);
    MV(r, p, enu);
}


// 回転行列X
void geo::Rx(double theta, double (*mat)[3]) {
    mat[0][0] = 1.0;
    mat[0][1] = 0.0;
    mat[0][2] = 0.0;
    mat[1][0] = 0.0;
    mat[1][1] = cos(theta);
    mat[1][2] = sin(theta);
    mat[2][0] = 0.0;
    mat[2][1] = -sin(theta);
    mat[2][2] = cos(theta);
}


// 回転行列Y
void geo::Ry(double theta, double (*mat)[3]) {
    mat[0][0] = cos(theta);
    mat[0][1] = 0.0;
    mat[0][2] = -sin(theta);
    mat[1][0] = 0.0;
    mat[1][1] = 1.0;
    mat[1][2] = 0.0;
    mat[2][0] = sin(theta);
    mat[2][1] = 0.0;
    mat[2][2] = cos(theta);
}


// 回転行列Z
void geo::Rz(double theta, double (*mat)[3]) {
    mat[0][0] = cos(theta);
    mat[0][1] = sin(theta);
    mat[0][2] = 0.0;
    mat[1][0] = -sin(theta);
    mat[1][1] = cos(theta);
    mat[1][2] = 0.0;
    mat[2][0] = 0.0;
    mat[2][1] = 0.0;
    mat[2][2] = 1.0;
}


// 行列(3,3)*行列(3,3) a*b=answer
void geo::MM(double (*mat_a)[3], double (*mat_b)[3], double (*mat_answer)[3]) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                mat_answer[i][j] += mat_a[i][k] * mat_b[k][j];
            }
        }
    }
}


// 行列(3,3)*ベクトル(3,1) a*b=answer
void geo::MV(double (*mat_a)[3], double *mat_b, double *mat_answer) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            mat_answer[i] += mat_a[i][j] * mat_b[j];
        }
    }
}


// angle(degree)を計算する(east, northより)
double geo::angle(double east, double north) {
    double angle;
    double r = atan2(north, east) * 180.0 / M_PI;
    
    if (east < 0 && north >= 0)
        angle = 270.0 + (180.0 - r);
    else
        angle = 90.0 - r;
    
    return angle;
}


// distance(m)を計算する(east, northより)
double geo::distance(double east, double north) {
    double distance;
    distance = sqrt(east * east + north * north);
    return distance;
}