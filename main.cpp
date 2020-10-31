#include <engine/common.h>
#include <engine/renderer.h>
#include <engine/meshloader.h>

int amount = 2;

void shaderObjectFunc(Object& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");
}
void drawFrame(Renderer& r){
    Object* currObj;
    ///////////////////////////////
    r.bindFramebufferByIndex(0);

    currObj = r.getObjectByIndex(0);
    currObj->draw(shaderObjectFunc);

    for(int i = 1; i < amount; i++){
        currObj = r.getObjectByIndex(i);
        currObj->draw(shaderObjectFunc);
    }

    r.skyboxes->at(0)->skyboxTexture->pushToShader(currObj->shader, 0, "skybox");

    r.skyboxes->at(0)->draw();

    r.bindDefaultFramebuffer();
    ///////////////////////////////

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

    setEvent(r.window->getWindowPtr(), F, r.lightSources->getByName("default")->lightPos = r.view->getCamera().position);
}

int main (){
    int window_width = 1366;
    int window_height = 768;

    Renderer renderer(window_width, window_height);
    MeshLoader meshLoader;
    MeshList meshList;
    ShaderList shaders;
    TextureList tex, tex1;
    MaterialList materials;

    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\neptune\\neptune.obj", "loaded"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\skybox.obj", "skybox"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\quad.obj", "quad"));

    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\obj_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\obj_fragment.fsh", "default");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_fragment.fsh", "skybox");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_fragment.fsh", "screen");

    tex.loadNew("C:/EngPathReq/might_beeeeeeeeeeee/pictures/box.png");
    tex.loadNew("C:/EngPathReq/might_beeeeeeeeeeee/pictures/box1.png");

    tex.addLayouts(1);
    tex.appendTextureToLayout(0, 0, 0, "texture0");
    tex.appendTextureToLayout(0, 1, 1, "specularTex");

    tex1.loadNew("C://EngPathReq//might_beeeeeeeeeeee//models//neptune//Texf_mouse.png");
    tex1.loadNew("C://EngPathReq//might_beeeeeeeeeeee//models//neptune//Tex002f_body01.png");
    tex1.loadNew("C://EngPathReq//might_beeeeeeeeeeee//models//neptune//Tex002f_body02.png");
    tex1.loadNew("C://EngPathReq//might_beeeeeeeeeeee//models//neptune//Texf_body02.png");
    tex1.loadNew("C://EngPathReq//might_beeeeeeeeeeee//models//neptune//Tex001f_eye.png");

    tex1.addLayouts(4);
    tex1.appendTextureToLayout(0, 0, 0, "texture0"); //mouse
    tex1.appendTextureToLayout(0, 1, 0, "specularTex"); //mouse
    tex1.appendTextureToLayout(1, 0, 1, "texture0"); //body
    tex1.appendTextureToLayout(1, 1, 2, "specularTex"); //body
    tex1.appendTextureToLayout(2, 0, 3, "texture0"); //body02
    tex1.appendTextureToLayout(2, 1, 3, "specularTex"); //body02
    tex1.appendTextureToLayout(3, 0, 4, "texture0"); //eye
    tex1.appendTextureToLayout(3, 1, 4, "specularTex"); //eye

    Buffer buffer(*meshList.getByName("loaded"), *shaders.getByName("default"));
    Buffer skyboxCube(*meshList.getByName("skybox"), *shaders.getByName("skybox"));
    Buffer quad(*meshList.getByName("quad"), *shaders.getByName("screen"));

    Material* mat = new Material;
    mat->setAmbientColor(0.01f,0.01f,0.01f);
    mat->setSpecularHighlights(300);

    materials.push(*mat);

    Framebuffer framebuffer(window_width, window_height);
    framebuffer.genTextureColorBuffers(1);
    renderer.addFramebuffer(framebuffer);

    renderer.addNewLightSource(0,0,2,"default");

    int x = 0;
    for(int i = 0; i < amount; i++){
        renderer.addNewObject(tex1, buffer, &materials);
        renderer.getObjectByIndex(i)->moveTo(x,0,0);
        renderer.getObjectByIndex(i)->scaleTo(0.01,0.01,0.01);
        x+= 2.2f;
    }

    renderer.addNewObject(*framebuffer.textureColorBuffers, quad, &materials, "screen");
    renderer.getObjectByName("screen")->rotate(0,180,0);

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
