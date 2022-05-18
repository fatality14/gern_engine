#pragma once

#include <common/common.h>

class JointPosition : public ICommon{
public:
    glm::vec3 tip;
    glm::vec3 origin;

    glm::vec3 location;
    glm::vec3 rotation;
    glm::vec3 scale;

    JointPosition(){
        location = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        transform = glm::mat4(1.f);
        parentTransform = glm::mat4(1.f);
    }

    void updateMatrices(){
        transform = parentTransform;

        glm::quat q(glm::radians(rotation));
        transform = glm::translate(transform, origin);
        transform = glm::translate(transform, location);
        transform = transform * glm::toMat4(q);
        transform = glm::scale(transform, glm::vec3(scale));
        transform = glm::translate(transform, -origin);
    }

    void move(float x, float y, float z){
        location.x += x;
        location.y += y;
        location.z += z;
        updateMatrices();
    }
    void move(glm::vec3 location){
        this->location += location;
        updateMatrices();
    }
    void moveTo(float x, float y, float z){
        location.x = x;
        location.y = y;
        location.z = z;
        updateMatrices();
    }
    void moveTo(glm::vec3 location){
        this->location = location;
        updateMatrices();
    }
    void rotate(float x, float y, float z){
        rotation.x += x;
        rotation.y += y;
        rotation.z += z;
        updateMatrices();
    }
    void rotate(glm::vec3 rotation){
        this->rotation += rotation;
        updateMatrices();
    }
    void rotateTo(float x, float y, float z){
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
        updateMatrices();
    }
    void rotateTo(glm::vec3 rotation){
        this->rotation = rotation;
        updateMatrices();
    }
    void scaleBy(float x, float y, float z){
        scale.x += x;
        scale.y += y;
        scale.z += z;
        updateMatrices();
    }
    void scaleBy(glm::vec3 scale){
        this->scale += scale;
        updateMatrices();
    }
    void scaleTo(float x, float y, float z){
        scale.x = x;
        scale.y = y;
        scale.z = z;
        updateMatrices();
    }
    void scaleTo(glm::vec3 scale){
        this->scale = scale;
        updateMatrices();
    }

    glm::mat4 getModelMatrix(){
        return transform;
    }
    glm::vec3 getLocation(){
        return location;
    }
    glm::vec3 getRotation(){
        return rotation;
    }
    glm::vec3 getScale(){
        return scale;
    }
    void setParentTransform(glm::mat4 transform){
        parentTransform = transform;
    }
private:
    glm::mat4 transform;
    glm::mat4 parentTransform;
};
