#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/mouselistener.h>
#include <engine/camera.h>
#include <engine/shader.h>

class Position{
public:
    Position(){
        location = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        modelMatrix = glm::mat4(1.f);
    }

    void pushToShader(Shader* s, string uniformName){
        s->setUniformMatrix4fv(uniformName, glm::value_ptr(modelMatrix));
    }
    void updateMatrices(){
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, location);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    }

    void setDefaultEvents(Window* w){
        setEvent(w->getWindowPtr(), 2, scale -= 0.01f);
        setEvent(w->getWindowPtr(), 1, scale += 0.01f);
        setEvent(w->getWindowPtr(), Z, rotation.y -= 5.f);
        setEvent(w->getWindowPtr(), X, rotation.y += 5.f);
        updateMatrices();
    }
    glm::mat4& getModelMatrix(){
        return modelMatrix;
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
    glm::vec3 getLocation(){
        return location;
        updateMatrices();
    }
    glm::vec3 getRotation(){
        return rotation;
        updateMatrices();
    }
    glm::vec3 getScale(){
        return scale;
        updateMatrices();
    }
private:
    glm::mat4 modelMatrix;

    glm::vec3 location;
    glm::vec3 rotation;
    glm::vec3 scale;
};
