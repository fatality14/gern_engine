#pragma once

#include <common/common.h>

class Window : public ICommon{
public:
    int width;
    int height;

    int fbWidth;
    int fbHeight;

    GLenum pmFace;
    GLenum pmMode;

    string name;

    static bool isWindowInit;

    Window(int width, int height, GLenum pmFace = GL_FRONT_AND_BACK, GLenum pmMode = GL_FILL, string windowName = "default"){
        if(isWindowInit){
            cout << "Only one window allowed for the process\n";
            exit(0);
        }

        this->width = width;
        this->height = height;
        fbWidth = width;
        fbHeight = height;
        this->pmFace = pmFace;
        this->pmMode = pmMode;
        this->name = windowName;

        isWindowInit = true;
        initGLFW();
        initWindow();
    }
    Window(Window& w) = delete;
    ~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
        isWindowInit = false;
    }

    void setPolygonMode(GLenum face = GL_FRONT_AND_BACK, GLenum mode = GL_FILL){
        GLDB(glPolygonMode(face, mode));
    }
    void setDrawOrder(bool isCounterclockwise){
        if(isCounterclockwise){
            GLDB(glFrontFace(GL_CCW));
        }
        else{
            GLDB(glFrontFace(GL_CW));
        }
    }
    GLFWwindow* getWindowPtr(){
        return window;
    }
private:
    GLFWwindow* window;

    void initGLFW(){
        if (!glfwInit())
        {
            cout << "Cannot initialize glfw" << endl;
            glfwTerminate();
        }

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    }
    void initWindowData(){
        window = glfwCreateWindow(width, height, name.data(), NULL, NULL);

        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glfwMakeContextCurrent(window);
    }
    void initGLEW(){
        //glViewport(0 ,0, fbWidth, fbHeight);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK){
            cout << "Cannot initialize glew" << endl;
            glfwTerminate();
        }

        GLDB(glEnable(GL_DEPTH_TEST));
        GLDB(glEnable(GL_CULL_FACE));
        GLDB(glCullFace(GL_BACK));
        GLDB(glFrontFace(GL_CCW));
        GLDB(glEnable(GL_BLEND));
        GLDB(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    void initWindow(){
        initWindowData();
        initGLEW();
        setPolygonMode(pmFace, pmMode);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH){
        GLDB(glViewport(0, 0, fbW, fbH));
    }
};

bool Window::isWindowInit = false;
