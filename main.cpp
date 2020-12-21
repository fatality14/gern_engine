#include <engine/common.h>
#include <engine/renderer.h>
#include <engine/meshloader.h>
#include <engine/skeletonobject.h>
#include <engine/skeletizer.h>

int amount = 1;

void shaderObjectFunc(Object& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void shaderSklObjectFunc(SkeletonObject& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void drawFrame(Renderer& r){
    static Object* currObj;
    static SkeletonObject* currSklObj;
    ///////////////////////////////
    r.bindFramebufferByIndex(0, 1);

    currSklObj = r.getSkeletonObjectByName("paimon");
    currSklObj->draw(shaderSklObjectFunc);

    currObj = r.getObjectByIndex(1);
    currObj->position->moveTo((r.view->getCamera().location + r.view->getCamera().front));
    currObj->draw(shaderObjectFunc);

//    r.skyboxes->at(0)->skyboxTexture->pushToShader(currObj->shader, 0, "skybox");
    r.skyboxes->at(0)->draw();
    ///////////////////////////////
    r.bindDefaultFramebuffer();

    currObj = r.getObjectByName("screen");
    currObj->draw(shaderObjectFunc);
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
    MeshLoader meshLoader;
    MeshList meshList;
    SkeletonMeshList skeletonList;
    Skeletizer skeletizer;
    ShaderList shaders;
    TextureList tex;
    MaterialList materials;

    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\skybox.obj", "skybox"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\quad.obj", "quad"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\sphere.obj", "sphere"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\paimon.obj", "paimon"));

    skeletonList.push(skeletizer.skeletize(*meshList.getByName("paimon"),
                                           "C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\paimon.skl"));

    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\obj_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\obj_fragment.fsh", "default");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_fragment.fsh", "skybox");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_fragment.fsh", "screen");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skeleton_obj_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skeleton_obj_fragment.fsh", "skeleton");

    tex.loadNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\Texture\\body.jpg");//0
    tex.loadNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\Texture\\face.jpg");//1
    tex.loadNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\Texture\\hair.jpg");//2
    tex.loadNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\Texture\\eyes.png");//3
    tex.loadNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\Texture\\cloak.jpg");//4

    tex.addLayouts(8);
    tex.appendTextureToLayout(0, 0, 1, "texture0");//eyes back
    tex.appendTextureToLayout(1, 0, 2, "texture0");//eyes front
    tex.appendTextureToLayout(2, 0, 1, "texture0");//face
    tex.appendTextureToLayout(3, 0, 0, "texture0");//перчатки чулки
    tex.appendTextureToLayout(4, 0, 0, "texture0");//тело
    tex.appendTextureToLayout(5, 0, 2, "texture0");//hair
    tex.appendTextureToLayout(6, 0, 4, "texture0");//плащ
    tex.appendTextureToLayout(7, 0, 0, "texture0");//?

    Buffer skyboxCube(*meshList.getByName("skybox"), *shaders.getByName("skybox"));
    Buffer quad(*meshList.getByName("quad"), *shaders.getByName("screen"));
    Buffer sphere(*meshList.getByName("sphere"), *shaders.getByName("default"));

    SkeletonBuffer paimonskl(*skeletonList.getByName("paimon"), *shaders.getByName("skeleton"));

    Material* mat = new Material;
    mat->setAmbientColor(0.01f,0.01f,0.01f);
    mat->setSpecularHighlights(300);

    materials.push(*mat);

    Framebuffer framebuffer(window_width, window_height);
    framebuffer.genTextureColorBuffers(2);
    renderer.addFramebuffer(framebuffer);

    renderer.addNewLightSource(0,0,2,"default");

    renderer.addNewObject(*framebuffer.textureColorBuffers, quad, &materials, "screen");
    renderer.getObjectByName("screen")->rotate(0,180,0);

    renderer.addNewObject(tex, sphere, &materials, "sphere");
    renderer.getObjectByName("sphere")->scaleTo(0.01,0.01,0.01);

    renderer.addNewSkeletonObject(tex, paimonskl, &materials, "paimon");
    renderer.getSkeletonObjectByIndex(0)->setAnimation("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\GenshinImpactPack\\paimon\\paimon.anm", 3);
//    renderer.getSkeletonObjectByIndex(0)->startAnimation();

    vector<string> skyboxSides;
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\right.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\left.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\top.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\bottom.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\front.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\back.jpg");

    renderer.addNewSkybox(skyboxSides, skyboxCube);
    renderer.getSkyboxObjectByIndex(0)->scaleTo(10,10,10);

    renderer.setBackgroundColor(0.5f, 0.f, 0.f, 0.99f);
    renderer.render(drawFrame);
    return 0;
}
