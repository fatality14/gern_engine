#pragma once

#include <animation/animation.h>
#include <animation/poseloader.h>
#include <buffer/skeletonbuffer.h>
#include <object/aobject.h>
#include <shader/lightsource.h>
#include <texture/material.h>

class SkeletonObject : public AObject<SkeletonBuffer> {
public:
    Animation* currAnimation;
    AnimationList animations;
    LightSourceList* lightSources;
    MaterialList* materials;
    TextureList* texList;

    bool doAnimation = false;

    inline static GLuint currShaderId = -1;

    // make arguments optional
    SkeletonObject(Window& w, SkeletonBuffer& b, Perspective& p, View& v,
                   LightSourceList& lsl, MaterialList& ml,
                   const string& name = "noname")
        : AObject(w, b, p, v, name) {
        lightSources = &lsl;
        materials = &ml;
        texList = materials->textures;

        shaderFields.push(buffer->getMesh().joints);
        shaderFields.push(*position);
        shaderFields.push(*perspective);
        shaderFields.push(*view);
    }

    void addAnimation(Animation& a) { animations.push(a); }

    void popAnimationByIndex(size_t index) { animations.popByIndex(index); }
    void popAnimationByName(const string& name) { animations.popByName(name); }

    void setCurrAnimation(size_t index) {
        currAnimation = animations.at(index);
    }
    void setCurrAnimation(const string& name) {
        currAnimation = animations.getByName(name);
    }

    void stopAnimation() { doAnimation = false; }
    void startAnimation() { doAnimation = true; }

    void draw(int flags = 0) {
        SkeletonMesh* currMesh = &buffer->getMesh();

        shader->bind();

        position->setDefaultEvents(*window);

        if (currShaderId != shader->program) {
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);
            lightSources->pushToShader(*shader);

            currShaderId = shader->program;
        }
        if (flags == 0) {
            shaderFields.pushToShader(*shader);
        }

        if (doAnimation) {
            currAnimation->applyCurrPose();
        }

        buffer->bind();

        // cout << "Draw mesh: " << currMesh->name << endl;
        GLint startFrom = 0;
        size_t textureI = 0;
        size_t materialI = 0;

        for (size_t j = 0; j < currMesh->partEndMtlIds.size(); ++j) {
            if (textureI == texList->layoutsAmount()) {
                textureI = 0;
            }
            texList->setShaderParams(textureI);
            texList->pushToShader(*shader);
            ++textureI;

            if (materialI == materials->size()) {
                materialI = 0;
            }
            materials->at(materialI)->pushToShader(*shader);
            ++materialI;

            if (j == 0) {
                GLDB(glDrawArrays(drawmode, startFrom,
                                  currMesh->partEndMtlIds.at(j)));
                startFrom = currMesh->partEndMtlIds.at(j);
            } else {
                GLDB(glDrawArrays(drawmode, startFrom,
                                  currMesh->partEndMtlIds.at(j) -
                                      currMesh->partEndMtlIds.at(j - 1)));
                startFrom = currMesh->partEndMtlIds.at(j);
            }
        }

        buffer->unbind();
        shader->unbind();
        texList->unbindTextures();
    }
};

class SkeletonObjects : public AListO<SkeletonObject> {};
