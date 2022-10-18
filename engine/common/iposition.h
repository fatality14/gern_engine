#pragma once

#include <common/common.h>

class IPosition : public ICommon {
public:
    virtual void move(float x, float y, float z) = 0;
    virtual void move(glm::vec3 location) = 0;
    virtual void moveTo(float x, float y, float z) = 0;
    virtual void moveTo(glm::vec3 location) = 0;
    virtual void rotate(float x, float y, float z) = 0;
    virtual void rotate(glm::vec3 rotation) = 0;
    virtual void rotateTo(float x, float y, float z) = 0;
    virtual void rotateTo(glm::vec3 rotation) = 0;
    virtual void scaleBy(float x, float y, float z) = 0;
    virtual void scaleBy(glm::vec3 scale) = 0;
    virtual void scaleTo(float x, float y, float z) = 0;
    virtual void scaleTo(glm::vec3 scale) = 0;
};