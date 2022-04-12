#pragma once

#include <aobject.h>
#include <buffer.h>
#include <lightsource.h>
#include <material.h>

class Object : public AObject<Buffer>{
public:
    LightSourceList* lightSources;
    MaterialList* materials;//move to mesh

    //make arguments optional
    Object(Window& w, TextureList& t,
           Buffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           MaterialList& ml, string name = "noname")
        : AObject(w, t, b, p, v, name)
    {
        lightSources = &lsl;
        materials = &ml;

        shaderFields.push(*position);
        shaderFields.push(*perspective);
        shaderFields.push(*view);
    }

    void draw(int flags = 0) override{
        shader->bind();

        position->setDefaultEvents(window);

        if(currShaderId != shader->program){
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);

            currShaderId = shader->program;
        }
        if(flags == 0){
            shaderFields.pushToShader(*shader);
        }


        buffer->bind();

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

    static GLuint currShaderId;
};

GLuint Object::currShaderId = -1;

class Objects : public AList<Object>{

};
