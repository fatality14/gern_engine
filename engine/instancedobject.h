#pragma once

#include <aobject.h>
#include <instancedbuffer.h>
#include <lightsource.h>
#include <material.h>

class InstancedObject : public AObject<InstancedBuffer>{
public:
    LightSourceList* lightSources;
    Materials* materials;//move to mesh
    TextureList* texList;

    vector<Position> modelMatrices;

    static GLuint currShaderId;

    //make arguments optional
    InstancedObject(Window& w, InstancedBuffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           Materials& ml, vector<Position> modelMatrices, string name = "noname")
        : AObject(w, b, p, v, name)
    {
        this->modelMatrices = modelMatrices;
        lightSources = &lsl;
        materials = &ml;
        texList = materials->textures;

        shaderFields.push(*view);
        shaderFields.push(*perspective);
        updateBufferModelMats();
    }

    void updateBufferModelMats(){
        buffer->setModelMatrices(modelMatrices);
    }

    void setTextureList(TextureList& tl){
        texList = &tl;
    }
    TextureList& getTextureList(){
        return *texList;
    }

    void draw(int flags = 0) override{
        shader->bind();

        if(currShaderId != shader->program){
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);

            currShaderId = shader->program;
        }

        //if different fields in comp with prev draw
        if(flags <= 1){
            shaderFields.pushToShader(*shader);
        }

        buffer->bind();

        //if there's need to update positions
        if(flags == 0){
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
                texList->setShaderParams(textureI);
                texList->pushToShader(*shader);
                ++textureI;

                if(materialI == materials->size()){
                    materialI = 0;
                }
                materials->at(materialI)->pushToShader(*shader);
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

class InstancedObjects : public AList<InstancedObject>{

};
