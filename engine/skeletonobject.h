#pragma once

#include <common.h>
#include <window.h>
#include <view.h>
#include <perspective.h>
#include <buffer.h>
#include <shader.h>
#include <position.h>
#include <texture.h>
#include <lightsource.h>
#include <material.h>
#include <skeletonbuffer.h>
#include <animation.h>

class SkeletonObject{
public:
    Window* window;
    TextureList* texList;
    SkeletonBuffer* buffer;
    Shader* shader;//shader same as in buffer
    Position* position;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;
    Animation* animation;
    MaterialList* materials;//move to mesh

//    vector<glm::mat4> transitions;//add Joint class hierarchy and remove transitions from here

    string name;
    bool doAnimation = false;

    static GLuint drawmode;
    static GLuint currShaderId;

    //make arguments optional
    SkeletonObject(Window& w, TextureList& t,
           SkeletonBuffer& b, Perspective& p,
           View& v, LightSourceList& lsl,
           MaterialList& ml, string name = "noname")
    {
        if(w.__getWindowPtr() != p.__getWindowPtr() || p.__getWindowPtr() != v.__getWindowPtr()){
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
        animation = new Animation(buffer->getMesh().joints);
        materials = &ml;

        this->name = name;

        position = new Position();
    }

    static void setDrawMode(GLuint GL_drawmode){
        drawmode = GL_drawmode;
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
    void draw(void (*shaderPassFunction)(SkeletonObject&), bool isSameShaderPassFunctionAsPrevCall = false){
        SkeletonMesh* currMesh = &buffer->getMesh();

        shader->bind();

        position->pushToShader(shader, "modelMatrix");
        position->setDefaultEvents(window);
        currMesh->joints.pushToShader(shader);

        if(currShaderId != shader->program){
            currShaderId = shader->program;

//            shader->setUniformMatrix4fv("jointTransforms", transitions.data(), 50);
            perspective->pushToShader(shader, "projectionMatrix");
            view->pushToShader(shader, "viewMatrix", "cameraPos");
        }
        if(!isSameShaderPassFunctionAsPrevCall){
            shaderPassFunction(*this);
        }
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
GLuint SkeletonObject::currShaderId = -1;
GLuint SkeletonObject::drawmode = GL_TRIANGLES;

class SkeletonObjects : public List<SkeletonObject>{

};
