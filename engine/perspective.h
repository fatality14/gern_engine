#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/mouselistener.h>
#include <engine/camera.h>
#include <engine/shader.h>

class Perspective{
public:
    float fov = 90.f;
    float nearPlane = 0.001f;
    float farPlane = 1000.f;

    Perspective(Window& w){
        window = &w;

        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }

    void pushToShader(Shader* s, string uniformName){
        updateMatrices();
        s->setUniformMatrix4fv(uniformName, glm::value_ptr(projectionMatrix));
    }
    void updateMatrices(){
        glfwGetFramebufferSize(window->getWindowPtr(), &window->fbWidth, &window->fbHeight);
        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }
    const GLFWwindow* getWindowPtr(){
        return  window->getWindowPtr();
    }

    //TODO: fill with smth later
    void setDefaultEvents(){}
private:
    glm::mat4 projectionMatrix;

    Window* window;
};
