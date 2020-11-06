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
    MaterialList* materials;//move to mesh

    string name;

    static GLuint drawmode;
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

    static void setDrawMode(GLuint GL_drawmode){
        drawmode = GL_drawmode;
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
            glDrawElements(drawmode, buffer->getMesh().nIndices, GL_UNSIGNED_INT64_ARB, (void*)0);
        else{
            Mesh* currMesh = &buffer->getMesh();
            //cout << "Draw mesh: " << currMesh->name << endl;
            uint startFrom = 0;
            size_t textureI = 0;
            size_t materialI = 0;

            for(size_t j = 0; j < currMesh->partEndMtlIds.size() - someCounter; ++j){
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

                if(j == 0){
                    glDrawArrays(drawmode, startFrom, currMesh->partEndMtlIds.at(j));
                    startFrom = currMesh->partEndMtlIds.at(j);
                }
                else{
                    glDrawArrays(drawmode, startFrom, currMesh->partEndMtlIds.at(j) - currMesh->partEndMtlIds.at(j-1));
                    startFrom = currMesh->partEndMtlIds.at(j);
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
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
    void scaleTo(glm::vec3 scale){
        position->scaleTo(scale);
    }
};
GLuint Object::currShaderId = -1;
GLuint Object::drawmode = GL_TRIANGLES;

class Objects : public List<Object>{

};
