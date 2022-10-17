#pragma once

#include <common/common.h>

class ITexture : public ICommon {
public:
    virtual void loadTexture() = 0;
};
