#include <common.h>
#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletizer.h>
#include <shaderfield.h>
#include <instancedobject.h>
#include <sceneloader.h>

int amount = 2;
bool once = true;

void drawFrame(Renderer& r){
    static Object* currObj;
    static SkeletonObject* currSklObj;
    static InstancedObject* currInstObj;
    ///////////////////////////////

    //move that in draw function
    //it is needed to delete light pointer here
    if(once){
        for(size_t i = 0; i < r.objects->size(); ++i){
            currObj = r.objects->at(i);
            currObj->shaderFields.push(*currObj->lightSources->at(0));
        }
        for(size_t i = 0; i < r.skeletonObjects->size(); ++i){
            currSklObj = r.skeletonObjects->at(i);
            currSklObj->shaderFields.push(*currSklObj->lightSources->at(0));
        }
        for(size_t i = 0; i < r.instancedObjects->size(); ++i){
            currInstObj = r.instancedObjects->at(i);
            currInstObj->shaderFields.push(*currInstObj->lightSources->at(0));
        }
        once = false;
    }

//    for(size_t i = 0; i < r.objects->size(); ++i){
//        currObj = r.objects->at(i);
//        ((ShaderUniform<glm::vec3>*)currObj->shaderFields.getByName("lightPos0"))->setValue(currObj->lightSources->at(0)->lightPos);
//    }
//    for(size_t i = 0; i < r.skeletonObjects->size(); ++i){
//        currSklObj = r.skeletonObjects->at(i);
//        ((ShaderUniform<glm::vec3>*)currSklObj->shaderFields.getByName("lightPos0"))->setValue(currSklObj->lightSources->at(0)->lightPos);
//    }
//    for(size_t i = 0; i < r.instancedObjects->size(); ++i){
//        currInstObj = r.instancedObjects->at(i);
//        ((ShaderUniform<glm::vec3>*)currInstObj->shaderFields.getByName("lightPos0"))->setValue(currInstObj->lightSources->at(0)->lightPos);
//    }
    ///////////////////////////////
    r.bindFramebufferByIndex(0, 1);

    currInstObj = r.getInstancedObjectByName("paimon");
    currInstObj->draw();

    int am = 30;
    for(int i = 0; i < am; ++i){
        currInstObj->rotate(i, 0, 1, 0);
    }

    currSklObj = r.getSkeletonObjectByName("paimon");
    currSklObj->draw();

    currObj = r.getObjectByIndex(1);
    currObj->position->moveTo((r.view->getCamera().location + r.view->getCamera().front));
    currObj->draw();

//    r.skyboxes->at(0)->skyboxTexture->pushToShader(currObj->shader, 0, "skybox");
    r.skyboxes->at(0)->draw();
    ///////////////////////////////
    static unsigned char* image;
    static Texture* t = r.getFramebufferByName("screenbuff")->textureColorBuffers->getByName("textureColorBuffer1");

    r.bindDefaultFramebuffer();

    if(amount != 1){
        t->setNewTextureData(image);
    }

    currObj = r.getObjectByName("screen");
    currObj->draw();


    if(amount == 1){
        image = &t->loadDataFromShader();

        ++amount;
    }

    ///////////////////////////////
    static float defaultSpeed = r.view->getCamera().movementSpeed;

    bool isShiftPressed = false;
    setEvent(r.window->getWindowPtr(), LEFT_SHIFT, isShiftPressed = true);
    if(isShiftPressed){
        r.view->getCamera().movementSpeed = defaultSpeed * 2;
    }
    bool isCtrlPressed = false;
    setEvent(r.window->getWindowPtr(), LEFT_CONTROL, isCtrlPressed = true);
    if(isCtrlPressed){
        r.view->getCamera().movementSpeed = defaultSpeed / 3;
    }
    if(!isShiftPressed && !isCtrlPressed){
        r.view->getCamera().movementSpeed = defaultSpeed;
    }

    setEvent(r.window->getWindowPtr(), F, r.lightSources->getByName("lightPos0")->lightPos = r.view->getCamera().location);

    setEvent(r.window->getWindowPtr(), J, if(someCounter + 1 <= 19) someCounter += 1.f/10.f; cout << someCounter << endl);
    setEvent(r.window->getWindowPtr(), K, if(someCounter - 1 >= 0) someCounter -= 1.f/10.f; cout << someCounter << endl);

    glm::vec3 sphereLoc = r.getObjectByName("sphere")->position->getLocation();
    setEvent(r.window->getWindowPtr(), P, cout << sphereLoc.x << " " << sphereLoc.y << " " << sphereLoc.z << endl);

    setEvent(r.window->getWindowPtr(), 1, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,-1,0));
    setEvent(r.window->getWindowPtr(), 2, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,1,0));
    setEvent(r.window->getWindowPtr(), 3, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,0,-1));
    setEvent(r.window->getWindowPtr(), 4, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(0,0,1));
    setEvent(r.window->getWindowPtr(), 5, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(-1,0,0));
    setEvent(r.window->getWindowPtr(), 6, currSklObj->buffer->getMesh().joints.getById(someCounter).rotate(1,0,0));

    setEvent(r.window->getWindowPtr(), I, cout << currSklObj->buffer->getMesh().joints.genPoseInfo() << endl);
    setEvent(r.window->getWindowPtr(), U, currSklObj->buffer->getMesh().joints.setDafaultPose());
    setEvent(r.window->getWindowPtr(), Y, currSklObj->buffer->getMesh().joints.at(someCounter)->setDefaultPose());
}

int main (){
    int window_width = 1366;
    int window_height = 768;

    Renderer renderer(window_width, window_height);
    SceneLoader sl;
    sl.load("C:\\Users\\1234\\Documents\\qtprojs\\ogl\\scenes\\test.txt", renderer);

    renderer.renderLoop(drawFrame);
    return 0;
}
