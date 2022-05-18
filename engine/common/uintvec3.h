#pragma once

#include <common/common.h>

struct uintvec3 : public ICommon{
    uintvec3(uint x, uint y, uint z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    uint x;
    uint y;
    uint z;
};
