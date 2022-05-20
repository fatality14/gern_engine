#pragma once

#include <render/framemodel.h>

//move to another file like as FrameModel
class FrameController : public IFrameController<FrameModel>{
public:
    FrameController(FrameModel& model) : m(model){}

    FrameModel& m;

    int amount = 1;
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
//            t->setNewTextureData(image);
        }

        currObj = m.getMeshObject("screen");
        currObj->draw();


        if(amount == 1){
//            image = &t->loadDataFromShader();
//            ++amount;
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

        setEvent(m.window->getWindowPtr(), T, currSklObj->currAnimation->setStartTime(10); currSklObj->currAnimation->reset());
//        setEvent(m.window->getWindowPtr(), G, currSklObj->startAnimation());

        m.resetObjectShaderLastIds();
    }
};
