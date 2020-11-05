#pragma once

#include <engine/common.h>
#include <engine/position.h>

class Joint{
public:
    vector<Joint*> childs;
    Position* position;
    uint id;
    string name;

    glm::vec3 firstPos;
    glm::vec3 secondPos;

    Joint(string name = "noname"){
        this->name = name;
        position = new Position;
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
