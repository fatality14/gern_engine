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
    void scaleBy(float x, float y, float z){
        position->scaleBy(x,y,z);
        setChildTransforms();
    }
    void scaleBy(glm::vec3 scale){
        position->scaleBy(scale);
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
            childs.at(i)->position->setParentTransform(parentTransform);
            childs.at(i)->position->updateMatrices();
            childs.at(i)->setChildTransforms();
        }
    }
    void setDefaultPose(){
        moveTo(0,0,0);
        scaleTo(1,1,1);
        rotateTo(0,0,0);
    }
};

class JointList : public List<Joint>{
public:
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
    void appendFromRootJoint(Joint& j){
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
    string genPoseInfo(){
        string res = "p ";
        for(size_t i = 0; i < size(); ++i){
            res += to_string(at(i)->id) + ' ';
        }
        res += '\n';
        for(size_t i = 0; i < size(); ++i){
            res += "l ";
            res += to_string(at(i)->position->getLocation().x) + ' ' +
                   to_string(at(i)->position->getLocation().y) + ' ' +
                   to_string(at(i)->position->getLocation().z) + '\n';

            res += "r ";
            res += to_string(at(i)->position->getRotation().x) + ' ' +
                   to_string(at(i)->position->getRotation().y) + ' ' +
                   to_string(at(i)->position->getRotation().z) + '\n';

            res += "s ";
            res += to_string(at(i)->position->getScale().x) + ' ' +
                   to_string(at(i)->position->getScale().y) + ' ' +
                   to_string(at(i)->position->getScale().z) + '\n';
        }
        return res;
    }
    void setDafaultPose(){
        for(size_t i = 0; i < size(); ++i){
            at(i)->moveTo(0,0,0);
            at(i)->scaleTo(1,1,1);
            at(i)->rotateTo(0,0,0);
        }
    }
private:
    vector<float> jointPoses;
};
