#pragma once

#include <aobject.h>
#include <skeletonbuffer.h>
#include <animation.h>
#include <lightsource.h>
#include <material.h>

class SkeletonObject : public AObject<SkeletonBuffer>{
public:
    Animation* animation;
    LightSourceList* lightSources;
    Materials* materials;//move to mesh
    TextureList* texList;

    bool doAnimation = false;

    static GLuint currShaderId;

    //make arguments optional
    SkeletonObject(Window& w, SkeletonBuffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           Materials& ml, string name = "noname")
        : AObject(w, b, p, v, name)
    {
        animation = new Animation(buffer->getMesh().joints);
        lightSources = &lsl;
        materials = &ml;
        texList = materials->textures;

        shaderFields.push(buffer->getMesh().joints);
        shaderFields.push(*position);
        shaderFields.push(*perspective);
        shaderFields.push(*view);
    }

    void setAnimation(string animationFilePath, float animationTime){
        animation->parseKeyPoses(animationFilePath);
        animation->animationTime = animationTime;
    }
    void stopAnimation(){
        doAnimation = false;
    }
    void startAnimation(){
        doAnimation = true;
    }

    void draw(int flags = 0){
        SkeletonMesh* currMesh = &buffer->getMesh();

        shader->bind();

        position->setDefaultEvents(window);

        if(currShaderId != shader->program){
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);

            currShaderId = shader->program;            
        }

        shaderFields.pushToShader(*shader);

        if(doAnimation){
            animation->applyCurrPose();
        }

        buffer->bind();


        if (buffer->getMesh().nIndices != 0)
            glDrawElements(drawmode, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else{
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
};

GLuint SkeletonObject::currShaderId = -1;

class SkeletonObjects : public AList<SkeletonObject>{

};
