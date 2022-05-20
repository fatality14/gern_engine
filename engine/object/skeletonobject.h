#pragma once

#include <object/aobject.h>
#include <buffer/skeletonbuffer.h>
#include <animation/animation.h>
#include <shader/lightsource.h>
#include <texture/material.h>

class SkeletonObject : public AObject<SkeletonBuffer>{
public:
    Animation* currAnimation;
    AnimationList animations;
    LightSourceList* lightSources;
    MaterialList* materials;
    TextureList* texList;

    bool doAnimation = false;

    static GLuint currShaderId;

    //make arguments optional
    SkeletonObject(Window& w, SkeletonBuffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           MaterialList& ml, string name = "noname")
        : AObject(w, b, p, v, name)
    {
        lightSources = &lsl;
        materials = &ml;
        texList = materials->textures;

        shaderFields.push(buffer->getMesh().joints);
        shaderFields.push(*position);
        shaderFields.push(*perspective);
        shaderFields.push(*view);
    }

    void parseAndPushAnimation(string animationFilePath, float animationTimeMult = 1, float startTime = 0, string name = "noname"){
        currAnimation = new Animation(buffer->getMesh().joints, animationTimeMult, startTime, name);
        currAnimation->parseKeyPoses(animationFilePath);
        animations.push(*currAnimation);
    }

    void popAnimationByIndex(size_t index){
        animations.popByIndex(index);
    }
    void popAnimationByName(string name){
        animations.popByName(name);
    }

    void setCurrAnimation(size_t index){
        currAnimation = animations.at(index);
    }
    void setCurrAnimation(string name){
        currAnimation = animations.getByName(name);
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
        if(flags == 0){
            shaderFields.pushToShader(*shader);
        }

        if(doAnimation){
            currAnimation->applyCurrPose();
        }

        buffer->bind();


        if (buffer->getMesh().nIndices != 0)
            glDrawElements(drawmode, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else{
            //cout << "Draw mesh: " << currMesh->name << endl;
            GLint startFrom = 0;
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

class SkeletonObjects : public AListO<SkeletonObject>{

};
