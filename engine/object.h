#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/view.h>
#include <engine/perspective.h>
#include <engine/buffer.h>
#include <engine/shader.h>
#include <engine/position.h>
#include <engine/texture.h>
#include <engine/lightsource.h>
#include <engine/material.h>

class Object{
public:
    Window* window;
    TextureList* texList;
    Buffer* buffer;
    Shader* shader;//shader same as in buffer
    Position* position;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;
    MaterialList* materials;

    string name;

    static GLuint currShaderId;

    //make arguments optional
    Object(Window& w, TextureList& t,
           Buffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           MaterialList& ml, string name = "noname")
    {
        if(w.getWindowPtr() != p.getWindowPtr() || p.getWindowPtr() != v.getWindowPtr()){
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
        lightSources = &lsl;
        materials = &ml;

        this->name = name;

        position = new Position();
    }

    void draw(void (*shaderPassFunction)(Object&), bool isSameShaderPassFunctionAsPrevCall = false){
        shader->bind();

        position->pushToShader(shader, "modelMatrix");
        position->setDefaultEvents(window);

        if(currShaderId != shader->program){
            currShaderId = shader->program;

            perspective->pushToShader(shader, "projectionMatrix");
            view->pushToShader(shader, "viewMatrix", "cameraPos");
        }
        if(!isSameShaderPassFunctionAsPrevCall){
            shaderPassFunction(*this);
        }

        buffer->bind();


        if (buffer->getMesh().nIndices != 0)
            glDrawElements(GL_TRIANGLES, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else{
            Mesh* currMesh = &buffer->getMesh();
            //cout << "Draw mesh: " << currMesh->name << endl;
            int startFrom = 0;
            bool first = true;
            size_t textureI = 0;
            size_t materialI = 0;
            for(size_t i = 0; i < currMesh->partEndVertexIds.size(); ++i){
                if(textureI == texList->layoutsAmount()){
                    textureI = 0;
                }
                texList->pushLayoutToShader(textureI, shader);
                ++textureI;

                if(materialI == materials->size()){
                    materialI = 0;
                }
                materials->at(materialI)->pushToShader(shader, "material");
                ++materialI;

                if(first){
                    //cout << "Draw " << i << " part." << endl;
                    first = false;
                    glDrawArrays(GL_TRIANGLES, startFrom, currMesh->partEndVertexIds.at(i));
                }
                else{
                    //cout << "Draw " << i << " part." << endl;
                    glDrawArrays(GL_TRIANGLES, startFrom, currMesh->partEndVertexIds.at(i) - currMesh->partEndVertexIds.at(i-1));
                    startFrom = currMesh->partEndVertexIds.at(i);
                }
            }
        }

        buffer->unbind();
        shader->unbind();
        texList->unbindTextures();
    }

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
GLuint Object::currShaderId = -1;

class Objects : public List<Object>{

};
