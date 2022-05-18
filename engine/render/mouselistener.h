#pragma once

#include <common/common.h>
#include <render/window.h>

class MouseListener : public ICommon{
public:
    float dt;
    float currTime;
    float lastTime;

    double lastMouseX;
    double lastMouseY;

    double mouseX;
    double mouseY;

    double mouseOffsetX;
    double mouseOffsetY;

    MouseListener(Window& w){
        window = &w;

        dt = 0;
        currTime = 0;
        lastTime = 0;

        lastMouseX = 0;
        lastMouseY = 0;

        mouseX = 0;
        mouseY = 0;

        mouseOffsetX = 0;
        mouseOffsetY = 0;

        firstMouse = true;
    }

    void update(){
        updateDt();
        updateInput();
        //cout << dt << " " << mouseOffsetX << " " << mouseOffsetY << endl;
    }
private:
    Window* window;

    bool firstMouse;

    void updateDt(){
        currTime = static_cast<float>(glfwGetTime());
        dt = currTime - lastTime;
        lastTime = currTime;
    }
    void updateInput(){
        glfwGetCursorPos(window->getWindowPtr(), &mouseX, &mouseY);

        if(firstMouse){
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }

        mouseOffsetX = mouseX - lastMouseX;
        mouseOffsetY = lastMouseY - mouseY;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
};
