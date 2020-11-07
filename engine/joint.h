#pragma once

#include <engine/common.h>
#include <engine/jointposition.h>
#include <engine/shader.h>

class Joint{
public:
    vector<Joint*> childs;
    JointPosition* position;
    uint id;
    string name;

    Joint(string name = "noname"){
        this->name = name;
        position = new JointPosition;
    }

    void move(float x, float y, float z){
        position->move(x,y,z);
        setChildTransforms();
    }
    void move(glm::vec3 location){
        position->move(location);
        setChildTransforms();
    }
    void moveTo(float x, float y, float z){
        position->moveTo(x,y,z);
        setChildTransforms();
    }
    void moveTo(glm::vec3 location){
        position->moveTo(location);
        setChildTransforms();
    }
    void rotate(float x, float y, float z){
        position->rotate(x,y,z);
        setChildTransforms();
    }
    void rotate(glm::vec3 rotation){
        position->rotate(rotation);
        setChildTransforms();
    }
    void rotateTo(float x, float y, float z){
        position->rotateTo(x,y,z);
        setChildTransforms();
    }
    void rotateTo(glm::vec3 rotation){
        position->rotateTo(rotation);
        setChildTransforms();
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
        setChildTransforms();
    }
    void scaleTo(glm::vec3 scale){
        position->scaleTo(scale);
        setChildTransforms();
    }

    void setChildTransforms(){
        glm::mat4 parentTransform = position->getModelMatrix();
        for(size_t i = 0; i < childs.size(); i++){
            childs.at(i)->position->parentTransform = parentTransform;
            childs.at(i)->position->updateMatrices();
            childs.at(i)->setChildTransforms();
        }
    }
};

class JointList : public List<Joint>{
public:
    JointList(){

    }
    void pushToShader(Shader* s){
        jointPoses.clear();
        for(size_t i = 0; i < size(); ++i){
            float* mat = (float*)glm::value_ptr(at(i)->position->getModelMatrix());
            for(int j = 0; j < 16; ++j){
                jointPoses.push_back(mat[j]);
            }
        }
        for(size_t i = 0; i < 50 - size(); ++i){
            for(int j = 0; j < 16; ++j){
                jointPoses.push_back(0);
            }
        }
        s->setUniformMatrix4fv("jointTransforms", jointPoses.data(), size());
    }
    void appendEmptyJoints(size_t amount){
        for(size_t i = 0; i < amount; ++i){
            Joint* j = new Joint;
            push(*j);
        }
    }
    void appendFromRootJoint(Joint& j){
        wipe();
        push(j);
        for(size_t i = 0; i < j.childs.size(); ++i){
            appendFromRootJoint(*j.childs.at(i));
        }
    }

    Joint& getById(uint id){
        for(size_t i = 0; i < size(); ++i){
            if(at(i)->id == id)
                return *at(i);
        }
    }
private:
    vector<float> jointPoses;
};
