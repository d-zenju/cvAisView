//
//  init.cpp
//  cvAisView
//
//  Created by ZENJU Daisuke on 2015/12/07.
//  Copyright © 2015年 ZENJU Daisuke. All rights reserved.
//

#include "init.hpp"

void Property::imnumber(int num) {
    if (imnum_min < num && num < imnum_max)
        imnum = std::to_string(num);
}