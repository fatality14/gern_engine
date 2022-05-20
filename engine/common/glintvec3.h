#pragma once

#include <common/common.h>

struct GLintvec3 : public ICommon{
    GLintvec3(GLint x, GLint y, GLint z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    GLint x;
    GLint y;
    GLint z;
};
