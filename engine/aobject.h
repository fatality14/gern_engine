#pragma once

#include <common.h>
#include <window.h>
#include <view.h>
#include <perspective.h>
#include <shaderfield.h>
#include <position.h>
#include <texture.h>

class IObject : public ICommon{
public:
    virtual void draw(int flags = 0) = 0;
    virtual ~IObject(){};
};

template <class T>
class AObject : public IObject{
public:
    Window* window;
    TextureList* texList;//move to mesh? or use in MaterialList
    T* buffer;
    Shader* shader;//shader same as in buffer
    Position* position;
    View* view;
    Perspective* perspective;
    ShaderFieldList shaderFields;

    string name;

    GLuint drawmode = GL_TRIANGLES;

    //make arguments optional
    AObject(Window& w, TextureList& t,
           T& b, Perspective& p,
           View& v, string name = "noname")
    {
        if(w.getWindowPtr() != p.__getWindowPtr() || p.__getWindowPtr() != v.__getWindowPtr()){
            cout << "Perspective and view passed in \"Object\" constructor must have pointers to the same \"Window\" object\n";
            cout << "Object not created\n";
            return;
        }

        window = &w;
        texList = &t;
        buffer = &b;
        shader = &buffer->getShaderPtr();
        perspective = &p;
        view = &v;

        this->name = name;

        position = new Position();
    }
    virtual ~AObject(){
        delete position;
    }

    AObject(AObject& t) = delete;

    void setDrawMode(GLuint GL_drawmode){
        drawmode = GL_drawmode;
    }

    virtual void draw(int flags = 0) = 0;

    void setTextureList(TextureList& tl){
        texList = &tl;
    }
    TextureList& getTextureList(){
        return *texList;
    }
    const string& getDrawMeshName(){
        return buffer->getMeshName();
    }
    void move(float x, float y, float z){
        position->move(x,y,z);
    }
    void move(glm::vec3 location){
        position->move(location);
    }
    void moveTo(float x, float y, float z){
        position->moveTo(x,y,z);
    }
    void moveTo(glm::vec3 location){
        position->moveTo(location);
    }
    void rotate(float x, float y, float z){
        position->rotate(x,y,z);
    }
    void rotate(glm::vec3 rotation){
        position->rotate(rotation);
    }
    void rotateTo(float x, float y, float z){
        position->rotateTo(x,y,z);
    }
    void rotateTo(glm::vec3 rotation){
        position->rotateTo(rotation);
    }
    void scaleBy(float x, float y, float z){
        position->scaleBy(x,y,z);
    }
    void scaleBy(glm::vec3 scale){
        position->scaleBy(scale);
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
    void scaleTo(glm::vec3 scale){
        position->scaleTo(scale);
    }
};
