#pragma once

#include <render/iframecontroller.h>
#include <render/window.h>

template <class T, class U = IFrameController<T>>
class ARenderer : public ICommon {
public:
    // add methods to change exact shader: push uniform to or else
    Window* window;
    T* model;
    U* controller;

    float r, g, b, a;

    ARenderer(int windowWidth, int windowHeight) {
        static_assert(
            std::is_base_of<IFrameController<T>, U>::value,
            "Template parameter U must be derived from IFrameController");
        window = new Window(windowWidth, windowHeight);
        model = new T(*window);
        controller = new U(*model);
    }
    ~ARenderer() {
        delete model;
        delete controller;
        delete window;
    }

    void renderLoop() {
        doContinue = true;
        while (!glfwWindowShouldClose(&window->getWindow())) {
            glfwPollEvents();

            // get enter press event to fast close
            if (glfwGetKey(&window->getWindow(), GLFW_KEY_ENTER) ==
                GLFW_PRESS) {
                doContinue = false;
            }

            controller->frameFunction();

            glfwSwapBuffers(&window->getWindow());
            GLDB(glFlush());

            if (!doContinue) {
                glfwSetWindowShouldClose(&window->getWindow(), GLFW_TRUE);
                break;
            }
        }
    }

private:
    bool doContinue = false;
};
