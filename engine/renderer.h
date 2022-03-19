#pragma once

#include <common.h>
#include <window.h>
#include <view.h>
#include <perspective.h>
#include <buffer.h>
#include <shader.h>
#include <position.h>
#include <texture.h>
#include <lightsource.h>
#include <material.h>
#include <skybox.h>
#include <framebuffer.h>
#include <object.h>
#include <skeletonobject.h>
#include <instancedobject.h>

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
        skeletonObjects = new SkeletonObjects;
        instancedObjects = new InstancedObjects;
        skyboxes = new SkyboxList;
        framebuffers = new FramebufferList;
        lightSources = new LightSourceList;
    }
    ~Renderer(){
        delete perspective;
        delete view;
        delete mouse;
        delete camList;
        delete skyboxes;
        delete objects;
        delete skeletonObjects;
        delete instancedObjects;
        delete window;
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
    void addNewSkeletonObject(TextureList& tl, SkeletonBuffer& b, MaterialList* ml, string name = "noname"){
        SkeletonObject* o = new SkeletonObject(*window, tl, b, *perspective, *view, *lightSources, *ml, name);
        skeletonObjects->push(*o);
    }
    void addNewInstancedObject(TextureList& tl, InstancedBuffer& b, MaterialList* ml, vector<Position>& mm, string name = "noname"){
        InstancedObject* o = new InstancedObject(*window, tl, b, *perspective, *view, *lightSources, *ml, mm, name);
        instancedObjects->push(*o);
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

    SkeletonObject* getSkeletonObjectByIndex(size_t index){
        return skeletonObjects->at(index);
    }
    SkeletonObject* getSkeletonObjectByName(string name){
        return skeletonObjects->getByName(name);
    }
    void popSkeletonObjectByIndex(size_t index){
        skeletonObjects->popByIndex(index);
    }
    void popSkeletonObjectByName(string name){
        skeletonObjects->popByName(name);
    }

    InstancedObject* getInstancedObjectByIndex(size_t index){
        return instancedObjects->at(index);
    }
    InstancedObject* getInstancedObjectByName(string name){
        return instancedObjects->getByName(name);
    }
    void popInstancedObjectByIndex(size_t index){
        instancedObjects->popByIndex(index);
    }
    void popInstancedObjectByName(string name){
        instancedObjects->popByName(name);
    }

    SkyboxObject* getSkyboxObjectByIndex(unsigned int index){
        return skyboxes->at(index);
    }
    SkyboxObject* getSkyboxObjectByName(string name){
        return skyboxes->getByName(name);
    }
    void popSkyboxObjectByIndex(size_t index){
        skyboxes->popByIndex(index);
    }
    void popSkyboxObjectByName(string name){
        skyboxes->popByName(name);
    }

    Framebuffer* getFramebufferByIndex(size_t index){
        return framebuffers->at(index);
    }
    Framebuffer* getFramebufferByName(string name){
        return framebuffers->getByName(name);
    }
    void popFramebufferByIndex(size_t index){
        framebuffers->popByIndex(index);
    }
    void popFramebufferByName(string name){
        framebuffers->popByName(name);
    }
    void bindFramebufferByIndex(size_t index, GLint attachmentNum = 0){
        framebuffers->at(index)->bind();
        framebuffers->at(index)->bindTextureColorBuffer(attachmentNum);
        clearBuffers();
    }
    void bindFramebufferByName(string name, GLint attachmentNum = 0){
        framebuffers->getByName(name)->bind();
        framebuffers->getByName(name)->bindTextureColorBuffer(attachmentNum);
        clearBuffers();
    }
    void bindDefaultFramebuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        clearBuffers();
    }

    void renderLoop(void (*frameFunction)(Renderer&)){
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

            Object::currShaderId = -1;
            SkeletonObject::currShaderId = -1;
            InstancedObject::currShaderId = -1;
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
    SkeletonObjects* skeletonObjects;
    InstancedObjects* instancedObjects;
    bool doContinue = false;
};
