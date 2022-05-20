#pragma once

#include <common/common.h>
#include <render/iframecontroller.h>
#include <render/iframemodel.h>

#include <render/window.h>
#include <shader/view.h>
#include <shader/perspective.h>
#include <buffer/meshbuffer.h>
#include <shader/shader.h>
#include <shader/position.h>
#include <texture/texture.h>
#include <shader/lightsource.h>
#include <texture/material.h>
#include <object/skyboxobject.h>
#include <texture/framebuffer.h>
#include <object/meshobject.h>
#include <object/skeletonobject.h>
#include <object/instancedobject.h>

class FrameModel : public IFrameModel{
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

    FrameModel(Window& window){
        this->window = &window;
        perspective = new Perspective(*this->window);
        mouse = new MouseListener(*this->window);
        camList = new CameraList();
        camList->push(*(new Camera("default")));
        camList->at(0)->location.z += 3;
        view = new View(*this->window, *mouse, *camList->at(0));
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
    ~FrameModel(){
        delete perspective;
        delete view;
        delete mouse;
        delete camList;
        delete skyboxes;
        delete meshObjects;
        delete skeletonObjects;
        delete instancedObjects;
    }

    //make arguments optional
    void addNewLightSource(float x, float y, float z, string name = "noname"){
        LightSource* ls = new LightSource(name);
        ls->setPosition(x,y,z);
        lightSources->push(*ls);
    }
    void addNewObject(MeshBuffer& b, MaterialList* ml, string name = "noname"){
        for(size_t i = 0; i < ml->list.size(); ++i){
            ml->list.at(i)->setShaderParams("material");
        }
        MeshObject* o = new MeshObject(*window, b, *perspective, *view, *lightSources, *ml, name);
        meshObjects->push(*o);
    }
    void addNewObject(SkeletonBuffer& b, MaterialList* ml, string name = "noname"){
        for(size_t i = 0; i < ml->list.size(); ++i){
            ml->list.at(i)->setShaderParams("material");
        }
        SkeletonObject* o = new SkeletonObject(*window, b, *perspective, *view, *lightSources, *ml, name);
        skeletonObjects->push(*o);
    }
    void addNewObject(InstancedBuffer& b, MaterialList* ml, vector<Position>& mm, string name = "noname"){
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

    MeshObject* getMeshObject(size_t index){
        return meshObjects->at(index);
    }
    MeshObject* getMeshObject(string name){
        return meshObjects->getByName(name);
    }
    void popObject(size_t index){
        meshObjects->popByIndex(index);
    }
    void popObject(string name){
        meshObjects->popByName(name);
    }

    SkeletonObject* getSkeletonObject(size_t index){
        return skeletonObjects->at(index);
    }
    SkeletonObject* getSkeletonObject(string name){
        return skeletonObjects->getByName(name);
    }
    void popSkeletonObject(size_t index){
        skeletonObjects->popByIndex(index);
    }
    void popSkeletonObject(string name){
        skeletonObjects->popByName(name);
    }

    InstancedObject* getInstancedObject(size_t index){
        return instancedObjects->at(index);
    }
    InstancedObject* getInstancedObject(string name){
        return instancedObjects->getByName(name);
    }
    void popInstancedObject(size_t index){
        instancedObjects->popByIndex(index);
    }
    void popInstancedObject(string name){
        instancedObjects->popByName(name);
    }

    SkyboxObject* getSkyboxObject(unsigned int index){
        return skyboxes->at(index);
    }
    SkyboxObject* getSkyboxObject(string name){
        return skyboxes->getByName(name);
    }
    void popSkyboxObject(size_t index){
        skyboxes->popByIndex(index);
    }
    void popSkyboxObject(string name){
        skyboxes->popByName(name);
    }

    Framebuffer* getFramebuffer(size_t index){
        return framebuffers->at(index);
    }
    Framebuffer* getFramebuffer(string name){
        return framebuffers->getByName(name);
    }
    void popFramebuffer(size_t index){
        framebuffers->popByIndex(index);
    }
    void popFramebuffer(string name){
        framebuffers->popByName(name);
    }
    void bindFramebuffer(size_t index, GLint attachmentNum = 0){
        framebuffers->at(index)->bind();
        framebuffers->at(index)->bindTextureColorBuffer(attachmentNum);
        clearBuffers();
    }
    void bindFramebuffer(string name, GLint attachmentNum = 0){
        framebuffers->getByName(name)->bind();
        framebuffers->getByName(name)->bindTextureColorBuffer(attachmentNum);
        clearBuffers();
    }
    void bindDefaultFramebuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        clearBuffers();
    }
    void clearBuffers(){
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void resetObjectShaderLastIds(){
        MeshObject::currShaderId = -1;
        SkeletonObject::currShaderId = -1;
        InstancedObject::currShaderId = -1;
        SkyboxObject::currShaderId = -1;
    }
    void updateCommonData(){
        mouse->update();
        view->updateCameraPosition();
        perspective->setDefaultEvents();
        view->setDefaultEvents();
    }

    void setBackgroundColor(float r, float g, float b, float a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};