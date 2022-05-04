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

#include <synchapi.h>

class IFrameModel  : public ICommon{};

template<class T>
class IFrameController : public ICommon{
public:
    IFrameController(){
        static_assert(std::is_base_of<IFrameModel, T>::value, "Template parameter T must be derived from IFrameModel");
    }
    void frameFunction(T& data);
};

//move to another file like as FrameController
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
};

//move to another file like as FrameModel
class FrameController : public IFrameController<FrameModel>{
public:
    FrameController(FrameModel& model) : m(model){}

    FrameModel& m;

    int amount = 2;
    bool once = true;

    void frameFunction(){
        static MeshObject* currObj;
        static SkeletonObject* currSklObj;
        static InstancedObject* currInstObj;
        ///////////////////////////////

        //move that in draw function
        if(once){
            for(size_t i = 0; i < m.meshObjects->size(); ++i){
                currObj = m.meshObjects->at(i);
                currObj->shaderFields.push(*currObj->lightSources->at(0));
            }
            for(size_t i = 0; i < m.skeletonObjects->size(); ++i){
                currSklObj = m.skeletonObjects->at(i);
                currSklObj->shaderFields.push(*currSklObj->lightSources->at(0));
            }
            for(size_t i = 0; i < m.instancedObjects->size(); ++i){
                currInstObj = m.instancedObjects->at(i);
                currInstObj->shaderFields.push(*currInstObj->lightSources->at(0));
            }
            once = false;
        }
        ///////////////////////////////
        m.bindFramebuffer(0, 1);

        currInstObj = m.getInstancedObject("paimon");
        currInstObj->draw();

        int am = 30;
        for(int i = 0; i < am; ++i){
            currInstObj->rotate(i, 0, 1, 0);
        }

        currSklObj = m.getSkeletonObject("paimon");
        currSklObj->draw();

        currObj = m.getMeshObject(1);
        currObj->position->moveTo((m.view->getCamera().location + m.view->getCamera().front));
        currObj->draw();

    //    r.skyboxes->at(0)->skyboxTexture->pushToShader(currObj->shader, 0, "skybox");
        m.skyboxes->at(0)->draw();
        ///////////////////////////////
        static unsigned char* image;
        static Texture* t = m.getFramebuffer("screenbuff")->textureColorBuffers->getByName("textureColorBuffer1");

        m.bindDefaultFramebuffer();

        if(amount != 1){
            t->setNewTextureData(image);
        }

        currObj = m.getMeshObject("screen");
        currObj->draw();


        if(amount == 1){
            image = &t->loadDataFromShader();

            ++amount;
        }

        ///////////////////////////////
        m.updateCommonData();

        static float defaultSpeed = m.view->getCamera().movementSpeed;

        bool isShiftPressed = false;
        setEvent(m.window->getWindowPtr(), LEFT_SHIFT, isShiftPressed = true);
        if(isShiftPressed){
            m.view->getCamera().movementSpeed = defaultSpeed * 2;
        }
        bool isCtrlPressed = false;
        setEvent(m.window->getWindowPtr(), LEFT_CONTROL, isCtrlPressed = true);
        if(isCtrlPressed){
            m.view->getCamera().movementSpeed = defaultSpeed / 3;
        }
        if(!isShiftPressed && !isCtrlPressed){
            m.view->getCamera().movementSpeed = defaultSpeed;
        }

        setEvent(m.window->getWindowPtr(), F, m.lightSources->getByName("lightPos0")->lightPos = m.view->getCamera().location);

        setEvent(m.window->getWindowPtr(), J, if(someCounter + 1 <= 19) someCounter += 1.f/10.f; cout << someCounter << endl);
        setEvent(m.window->getWindowPtr(), K, if(someCounter - 1 >= 0) someCounter -= 1.f/10.f; cout << someCounter << endl);

        glm::vec3 sphereLoc = m.getMeshObject("sphere")->position->getLocation();
        setEvent(m.window->getWindowPtr(), P, cout << sphereLoc.x << " " << sphereLoc.y << " " << sphereLoc.z << endl);

        setEvent(m.window->getWindowPtr(), 1, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,-1,0));
        setEvent(m.window->getWindowPtr(), 2, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,1,0));
        setEvent(m.window->getWindowPtr(), 3, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,0,-1));
        setEvent(m.window->getWindowPtr(), 4, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,0,1));
        setEvent(m.window->getWindowPtr(), 5, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(-1,0,0));
        setEvent(m.window->getWindowPtr(), 6, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(1,0,0));

        setEvent(m.window->getWindowPtr(), I, cout << currSklObj->buffer->getMesh().joints.genPoseInfo() << endl);
        setEvent(m.window->getWindowPtr(), U, currSklObj->buffer->getMesh().joints.setDafaultPose());
        setEvent(m.window->getWindowPtr(), Y, currSklObj->buffer->getMesh().joints.at(someCounter)->setDefaultPose());

        m.resetObjectShaderLastIds();
    }
};

template<class T, class U = IFrameController<T>>
class ARenderer : public ICommon{
public:
    //add methods to change exact shader: push uniform to or else
    Window* window;
    T* model;
    U* controller;

    float r, g, b, a;

    ARenderer(int windowWidth, int windowHeight){
        static_assert(std::is_base_of<IFrameController<T>, U>::value,
                      "Template parameter U must be derived from IFrameController");
        window = new Window(windowWidth, windowHeight);
        model = new T(*window);
        controller = new U(*model);
    }
    ~ARenderer(){
        delete model;
        delete controller;
        delete window;
    }

    void renderLoop(){
        doContinue = true;
        while (!glfwWindowShouldClose(window->getWindowPtr()))
        {
            glfwPollEvents();

            //get enter press event to fast close
            if(glfwGetKey(window->getWindowPtr(), GLFW_KEY_ENTER) == GLFW_PRESS){
                doContinue = false;
            }

            controller->frameFunction();

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
    bool doContinue = false;
};

typedef ARenderer<FrameModel, FrameController> Renderer;
