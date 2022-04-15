#pragma once

#include <common.h>
#include <window.h>
#include <view.h>
#include <perspective.h>
#include <meshbuffer.h>
#include <shader.h>
#include <position.h>
#include <texture.h>
#include <lightsource.h>
#include <material.h>
#include <skyboxobject.h>
#include <framebuffer.h>
#include <meshobject.h>
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
    MeshObjectList* meshObjects;
    SkeletonObjects* skeletonObjects;
    InstancedObjects* instancedObjects;
    FramebufferList* framebuffers;
    LightSourceList* lightSources;

    float r, g, b, a;

    Renderer(int windowWidth, int windowHeight){
        window = new Window(windowWidth, windowHeight);
        perspective = new Perspective(*window);
        mouse = new MouseListener(*window);
        camList = new CameraList();
        camList->push(*(new Camera("default")));
        camList->at(0)->location.z += 3;
        view = new View(*window, *mouse, *camList->at(0));
        meshObjects = new MeshObjectList;
        skeletonObjects = new SkeletonObjects;
        instancedObjects = new InstancedObjects;
        skyboxes = new SkyboxList;
        framebuffers = new FramebufferList;
        lightSources = new LightSourceList;

        perspective->selfPushable = false;
        view->selfPushable = false;
        view->setShaderParams("viewMatrix", "cameraPos");
    }
    ~Renderer(){
        delete perspective;
        delete view;
        delete mouse;
        delete camList;
        delete skyboxes;
        delete meshObjects;
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
    void addNewMeshObject(MeshBuffer& b, MaterialList* ml, string name = "noname"){
        for(size_t i = 0; i < ml->list.size(); ++i){
            ml->list.at(i)->setShaderParams("material");
        }
        MeshObject* o = new MeshObject(*window, b, *perspective, *view, *lightSources, *ml, name);
        meshObjects->push(*o);
    }
    void addNewSkeletonObject(SkeletonBuffer& b, MaterialList* ml, string name = "noname"){
        for(size_t i = 0; i < ml->list.size(); ++i){
            ml->list.at(i)->setShaderParams("material");
        }
        SkeletonObject* o = new SkeletonObject(*window, b, *perspective, *view, *lightSources, *ml, name);
        skeletonObjects->push(*o);
    }
    void addNewInstancedObject(InstancedBuffer& b, MaterialList* ml, vector<Position>& mm, string name = "noname"){
        for(size_t i = 0; i < ml->list.size(); ++i){
            ml->list.at(i)->setShaderParams("material");
        }
        InstancedObject* o = new InstancedObject(*window, b, *perspective, *view, *lightSources, *ml, mm, name);
        instancedObjects->push(*o);
    }
    void addNewSkybox(vector<string> facePaths, MeshBuffer& b, string name = "noname"){
        SkyboxTexture* st = new SkyboxTexture(facePaths);
        st->setShaderParams(0, "skybox");
        SkyboxObject* so = new SkyboxObject(*window, *st, b, *perspective, *view, name);
        skyboxes->push(*so);
    }
    void addFramebuffer(Framebuffer& fb){
        framebuffers->push(fb);
    }

    MeshObject* getObjectByIndex(size_t index){
        return meshObjects->at(index);
    }
    MeshObject* getObjectByName(string name){
        return meshObjects->getByName(name);
    }
    void popObjectByIndex(size_t index){
        meshObjects->popByIndex(index);
    }
    void popObjectByName(string name){
        meshObjects->popByName(name);
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

            MeshObject::currShaderId = -1;
            SkeletonObject::currShaderId = -1;
            InstancedObject::currShaderId = -1;
            SkyboxObject::currShaderId = -1;
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
    bool doContinue = false;
};
