#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/view.h>
#include <engine/perspective.h>
#include <engine/buffer.h>
#include <engine/shader.h>
#include <engine/position.h>
#include <engine/texture.h>
#include <engine/lightsource.h>
#include <engine/material.h>
#include <engine/skybox.h>
#include <engine/framebuffer.h>
#include <engine/object.h>

class Renderer{
public:
    //add methods to change exact shader: push uniform to or else
    Window* window;
    Perspective* perspective;
    View* view;
    MouseListener* mouse;
    CameraList* camList;//void addCamera
    SkyboxList* skyboxes;
    FramebufferList* framebuffers;
    LightSourceList* lightSources;

    float r, g, b, a;

    Renderer(int windowWidth, int windowHeight){
        window = new Window(windowWidth, windowHeight);

        perspective = new Perspective(*window);
        mouse = new MouseListener(*window);
        camList = new CameraList();
        camList->push(*(new Camera("default")));
        view = new View(*window, *mouse, *camList->at(0));
        objects = new Objects;
        skyboxes = new SkyboxList;
        framebuffers = new FramebufferList;
        lightSources = new LightSourceList;
    }
    ~Renderer(){
        delete window;
        delete perspective;
        delete view;
        delete mouse;
        delete camList;
        delete skyboxes;
        delete objects;
    }

    //make arguments optional
    void addNewLightSource(float x, float y, float z, string name = "noname"){
        LightSource* ls = new LightSource(name);
        ls->setPosition(x,y,z);
        lightSources->push(*ls);
    }
    void addNewObject(TextureList& tl, Buffer& b, MaterialList* ml, string name = "noname"){
        Object* o = new Object(*window, tl, b, *perspective, *view, *lightSources, *ml, name);
        objects->push(*o);
    }
    void addNewSkybox(vector<string> facePaths, Buffer& b, string name = "noname"){
        SkyboxTexture* st = new SkyboxTexture(facePaths, name);
        SkyboxObject* so = new SkyboxObject(*st, *window, b, *perspective, *view);
        skyboxes->push(*so);
    }
    void addFramebuffer(Framebuffer& fb){
        framebuffers->push(fb);
    }

    Object* getObjectByIndex(size_t index){
        return objects->at(index);
    }
    Object* getObjectByName(string name){
        return objects->getByName(name);
    }
    void popObjectByIndex(size_t index){
        objects->popByIndex(index);
    }
    void popObjectByName(string name){
        objects->popByName(name);
    }

    SkyboxObject* getSkyboxObjectByIndex(unsigned int index){
        return skyboxes->at(index);
    }
    SkyboxObject* getSkyboxObjectByName(string name){
        return skyboxes->getByName(name);
    }
    void popSkyboxObjectByIndex(size_t index){
        objects->popByIndex(index);
    }
    void popSkyboxObjectByName(string name){
        objects->popByName(name);
    }

    void bindFramebufferByIndex(size_t index){
        framebuffers->at(index)->bind();
        clearBuffers();
    }
    void bindFramebufferByName(string name){
        framebuffers->getByName(name)->bind();
        clearBuffers();
    }
    void bindDefaultFramebuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        clearBuffers();
    }

    void render(void (*frameFunction)(Renderer&)){
        doContinue = true;
        while (!glfwWindowShouldClose(window->getWindowPtr()))
        {
            glfwPollEvents();

            //get enter press event to fast close
            if(glfwGetKey(window->getWindowPtr(), GLFW_KEY_ENTER) == GLFW_PRESS){
                doContinue = false;
            }

            mouse->update();
            perspective->setDefaultEvents();
            view->setDefaultEvents();

            frameFunction(*this);

            glfwSwapBuffers(window->getWindowPtr());
            glFlush();

            if(!doContinue){
                glfwSetWindowShouldClose(window->getWindowPtr(), GLFW_TRUE);
                break;
            }
        }
    }
    void setBackgroundColor(float r, float g, float b, float a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    void clearBuffers(){
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
private:
    Objects* objects;
    bool doContinue = false;
};
