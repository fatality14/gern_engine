#pragma once

#include <common.h>

class Window{
public:
    int width;
    int height;

    int fbWidth;
    int fbHeight;

    GLenum pmFace;
    GLenum pmMode;

    Window(int width, int height, GLenum pmFace = GL_FRONT_AND_BACK, GLenum pmMode = GL_FILL){
        this->width = width;
        this->height = height;
        fbWidth = width;
        fbHeight = height;
        this->pmFace = pmFace;
        this->pmMode = pmMode;

        init();
    }
    Window(Window& w) = delete;
    ~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void setPolygonMode(GLenum face = GL_FRONT_AND_BACK, GLenum mode = GL_FILL){
        glPolygonMode(face, mode);
    }
    void setDrawOrder(bool isCounterclockwise){
        if(isCounterclockwise){
            glFrontFace(GL_CCW);
        }
        else{
            glFrontFace(GL_CW);
        }
    }
    GLFWwindow* getWindowPtr(){
        return window;
    }
private:
    GLFWwindow* window;

    void initGLFW(){
        glfwInit();

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    }
    void initWindow(){
        window = glfwCreateWindow(width, height, "Window", NULL, NULL);

        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glfwMakeContextCurrent(window);
    }
    void initGLEW(){
        //glViewport(0 ,0, fbWidth, fbHeight);

        //init glew
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK){
            cout << "Im dead" << endl;
            glfwTerminate();
        }

        //setup glew
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    void init(){
        initGLFW();
        initWindow();
        initGLEW();
        setPolygonMode(pmFace, pmMode);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH){
        glViewport(0, 0, fbW, fbH);
    }
};
