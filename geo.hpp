//
//  geo.hpp
//  geo
//
//  Created by ZENJU Daisuke on 2015/12/24.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#ifndef geo_hpp
#define geo_hpp

#include <iostream>
#include <math.h>


namespace geo {
    
    // 定数,変数宣言
    const double A	= 6378137.0;
    const double ONE_F = 298.257223563;
    const double B	= A * (1.0 - 1.0 / ONE_F);
    const double E2	 = (1.0 / ONE_F) * (2 - (1.0 / ONE_F));
    const double ED2 = E2 * A * A / (B * B);
    
    // 楕円体高
    double height(double altitude, double geoid);

    // BLH to ECEF(x, y, z)
    double NN(double p);
    double blh2x(double latitude, double longitude, double height);
    double blh2y(double latitude, double longitude, double height);
    double blh2z(double latitude, double longitude, double height);
    void blh2ecef(double *latitude, double *longitude, double *height, double *ecef);
    
    // ECEF(x, y, z) to BLH
    double ecef2lat(double x, double y, double z);
    double ecef2lon(double x, double y);
    double ecef2hgt(double x, double y, double z);
    
    // ECEF(x, y, z) to ENU(x', y', z')
    void ecef2enu(double *own, double *other, double *enu);
    
    // 回転行列X, Y, Z
    void Rx(double theta, double (*mat)[3]);
    void Ry(double theta, double (*mat)[3]);
    void Rz(double theta, double (*mat)[3]);
    
    // 行列(3,3)*行列(3,3) a*b=answer
    void MM(double (*mat_a)[3], double (*mat_b)[3], double (*mat_answer)[3]);
    
    // 行列(3,3)*ベクトル(3,1) a*b=answer
    void MV(double (*mat_a)[3], double *mat_b, double *mat_answer);
    
    // angle(degree)
    double angle(double east, double north);
    
    // distance(m)
    double distance(double east, double north);
};

#endif /* geo_hpp */
