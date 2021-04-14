#pragma once

#include <engine/common.h>
#include <engine/window.h>
#include <engine/view.h>
#include <engine/perspective.h>
#include <engine/instancedbuffer.h>
#include <engine/shader.h>
#include <engine/position.h>
#include <engine/texture.h>
#include <engine/lightsource.h>
#include <engine/material.h>

class InstancedObject{
public:
    Window* window;
    TextureList* texList;
    InstancedBuffer* buffer;
    Shader* shader;//shader same as in buffer
    vector<Position> modelMatrices;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;
    MaterialList* materials;//move to mesh

    string name;

    static GLuint drawmode;
    static GLuint currShaderId;

    //make arguments optional
    InstancedObject(Window& w, TextureList& t,
           InstancedBuffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           MaterialList& ml, vector<Position> modelMatrices, string name = "noname")
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

        this->modelMatrices = modelMatrices;
        updateBufferModelMats();

        this->name = name;
    }

    static void setDrawMode(GLuint GL_drawmode){
        drawmode = GL_drawmode;
    }
    void updateBufferModelMats(){
        buffer->setModelMatrices(modelMatrices);
    }
    void draw(void (*shaderPassFunction)(InstancedObject&), bool isSameShaderPassFunctionAsPrevCall = false, bool isStaticObject = true){
        shader->bind();

        if(currShaderId != shader->program){
            currShaderId = shader->program;

            perspective->pushToShader(shader, "projectionMatrix");
            view->pushToShader(shader, "viewMatrix", "cameraPos");
        }
        if(!isSameShaderPassFunctionAsPrevCall){
            shaderPassFunction(*this);
        }

        buffer->bind();

        if(!isStaticObject){
            updateBufferModelMats();
        }

        if (buffer->getMesh().nIndices != 0)
            glDrawElements(drawmode, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else{
            Mesh* currMesh = &buffer->getMesh();
            //cout << "Draw mesh: " << currMesh->name << endl;
            uint startFrom = 0;
            size_t textureI = 0;
            size_t materialI = 0;

            for(size_t j = 0; j < currMesh->partEndMtlIds.size(); ++j){
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
                    glDrawArraysInstanced(drawmode, startFrom, currMesh->partEndMtlIds.at(j), modelMatrices.size());
                    startFrom = currMesh->partEndMtlIds.at(j);
                }
                else{
                    glDrawArraysInstanced(drawmode, startFrom,
                                          currMesh->partEndMtlIds.at(j) - currMesh->partEndMtlIds.at(j-1),
                                          modelMatrices.size());
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
    void setModelMatrices(vector<Position> mm){
        modelMatrices = mm;
    }
    vector<Position>& getModelMatrices(){
        return modelMatrices;
    }
    void move(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].move(x,y,z);
    }
    void move(size_t index, glm::vec3 location){
        if(index < modelMatrices.size())
            modelMatrices[index].move(location);
    }
    void moveTo(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].moveTo(x,y,z);
    }
    void moveTo(size_t index, glm::vec3 location){
        if(index < modelMatrices.size())
            modelMatrices[index].moveTo(location);
    }
    void rotate(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].rotate(x,y,z);
    }
    void rotate(size_t index, glm::vec3 rotation){
        if(index < modelMatrices.size())
            modelMatrices[index].rotate(rotation);
    }
    void rotateTo(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].rotateTo(x,y,z);
    }
    void rotateTo(size_t index, glm::vec3 rotation){
        if(index < modelMatrices.size())
            modelMatrices[index].rotateTo(rotation);
    }
    void scaleBy(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].scaleBy(x,y,z);
    }
    void scaleBy(size_t index, glm::vec3 scale){
        if(index < modelMatrices.size())
            modelMatrices[index].scaleBy(scale);
    }
    void scaleTo(size_t index, float x, float y, float z){
        if(index < modelMatrices.size())
            modelMatrices[index].scaleTo(x,y,z);
    }
    void scaleTo(size_t index, glm::vec3 scale){
        if(index < modelMatrices.size())
            modelMatrices[index].scaleTo(scale);
    }
};
GLuint InstancedObject::currShaderId = -1;
GLuint InstancedObject::drawmode = GL_TRIANGLES;

class InstancedObjects : public List<InstancedObject>{

};
