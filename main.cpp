#include <common.h>
#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletizer.h>
#include <shaderuniform.h>
#include <instancedobject.h>
#include <sceneloader.h>

int amount = 2;

void shaderObjectFunc(Object& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void shaderSklObjectFunc(SkeletonObject& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void shaderInstObjectFunc(InstancedObject& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void skyboxObjectFunc(SkyboxObject& o){}

void drawFrame(Renderer& r){
    static Object* currObj;
    static SkeletonObject* currSklObj;
    static InstancedObject* currInstObj;
    ///////////////////////////////
    r.bindFramebufferByIndex(0, 1);

    currInstObj = r.getInstancedObjectByName("paimon");
    currInstObj->draw(shaderInstObjectFunc, 2);

    int am = 30;
    for(int i = 0; i < am; ++i){
        currInstObj->rotate(i, 0, 1, 0);
    }

    currSklObj = r.getSkeletonObjectByName("paimon");
    currSklObj->draw(shaderSklObjectFunc);

    currObj = r.getObjectByIndex(1);
    currObj->position->moveTo((r.view->getCamera().location + r.view->getCamera().front));
    currObj->draw(shaderObjectFunc);

//    r.skyboxes->at(0)->skyboxTexture->pushToShader(currObj->shader, 0, "skybox");
    r.skyboxes->at(0)->draw(skyboxObjectFunc);
    ///////////////////////////////
    static unsigned char* image;
    static Texture* t = r.getFramebufferByName("screenbuff")->textureColorBuffers->getByName("textureColorBuffer1");

    r.bindDefaultFramebuffer();

    if(amount != 1){
        t->setNewTextureData(image);
    }

    currObj = r.getObjectByName("screen");
    currObj->draw(shaderObjectFunc);


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

    setEvent(r.window->getWindowPtr(), F, r.lightSources->getByName("default")->lightPos = r.view->getCamera().location);

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
