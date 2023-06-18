#pragma once

#include <common/common.h>
#include <render/mouselistener.h>
#include <render/window.h>
#include <shader/camera.h>
#include <shader/ishaderfield.h>

class Perspective : public IShaderField {
public:
    float fov = 90.f;
    float nearPlane = 0.001f;
    float farPlane = 1000.f;

    Perspective(Window& w) {
        window = &w;

        projectionMatrix = glm::perspective(
            glm::radians(fov),
            static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane,
            farPlane);
        name = "projectionMatrix";
    }

    void pushToShader(Shader& s) override {
        updateMatrices();
        s.setUniformMatrix4fv(name, glm::value_ptr(projectionMatrix));
    }
    void updateMatrices() {
        glfwGetFramebufferSize(&window->getWindow(), &window->fbWidth,
                               &window->fbHeight);
        projectionMatrix = glm::perspective(
            glm::radians(fov),
            static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane,
            farPlane);
    }
    const GLFWwindow& getWindow() { return window->getWindow(); }
    glm::mat4& getProjectionMatrix() { return projectionMatrix; }

    // TODO: fill with smth later or remove
    void setDefaultEvents() {}

private:
    glm::mat4 projectionMatrix;

    Window* window;
};
