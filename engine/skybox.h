#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/view.h>
#include <engine/perspective.h>
#include <engine/buffer.h>
#include <engine/shader.h>
#include <engine/position.h>
#include <engine/skyboxtexture.h>

class SkyboxObject{
public:
    SkyboxTexture* skyboxTexture;

    Window* window;
    View* view;
    Perspective* perspective;
    Buffer* buffer;
    Shader* shader;//same as in buffer
    Position* position;

    string name;

    SkyboxObject(SkyboxTexture& st, Window& w, Buffer& b, Perspective& p, View& v){
        skyboxTexture = &st;
        window = &w;
        buffer = &b;
        shader = &buffer->getShaderPtr();
        perspective = &p;
        view = &v;

        position = new Position();

        name = skyboxTexture->name;
    }
    SkyboxObject(SkyboxObject& t) = delete;

    void draw(){
        window->setDrawOrder(false);
        shader->bind();

        position->pushToShader(shader, "modelMatrix");
        perspective->pushToShader(shader, "projectionMatrix");
        view->pushToShader(shader, "viewMatrix", "cameraPos");
        skyboxTexture->pushToShader(shader, 0, "skybox");

        position->setDefaultEvents(window);//remove this later

        buffer->bind();

        if (buffer->getMesh().nIndices != 0)
            glDrawElements(GL_TRIANGLES, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else
            glDrawArrays(GL_TRIANGLES, 0, buffer->getMesh().nVertices);

        buffer->unbind();
        shader->unbind();
        window->setDrawOrder(true);
    }

    void setTexture(SkyboxTexture& st){
        skyboxTexture = &st;
    }
    SkyboxTexture& getTexture(){
        return *skyboxTexture;
    }
    void move(float x, float y, float z){
        position->move(x,y,z);
    }
    void moveTo(float x, float y, float z){
        position->moveTo(x,y,z);
    }
    void rotate(float x, float y, float z){
        position->rotate(x,y,z);
    }
    void rotateTo(float x, float y, float z){
        position->rotateTo(x,y,z);
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
};

class SkyboxList : public List<SkyboxObject>{

};