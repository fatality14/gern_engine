#pragma once

#include <common/iposition.h>
#include <animation/jointposition.h>
#include <common/common.h>
#include <shader/ishaderfield.h>

class Joint : public IPosition {
public:
    vector<Joint*> childs;
    JointPosition* position;
    size_t id;
    string name;

    Joint(const string& name = "noname") {
        this->name = name;
        position = new JointPosition;
    }

    void move(float x, float y, float z) override {
        position->move(x, y, z);
        setChildTransforms();
    }
    void move(glm::vec3 location) override {
        position->move(location);
        setChildTransforms();
    }
    void moveTo(float x, float y, float z) override {
        position->moveTo(x, y, z);
        setChildTransforms();
    }
    void moveTo(glm::vec3 location) override {
        position->moveTo(location);
        setChildTransforms();
    }
    void rotate(float x, float y, float z) override {
        position->rotate(x, y, z);
        setChildTransforms();
    }
    void rotate(glm::vec3 rotation) override {
        position->rotate(rotation);
        setChildTransforms();
    }
    void rotateTo(float x, float y, float z) override {
        position->rotateTo(x, y, z);
        setChildTransforms();
    }
    void rotateTo(glm::vec3 rotation) override {
        position->rotateTo(rotation);
        setChildTransforms();
    }
    void scaleBy(float x, float y, float z) override{
        position->scaleBy(x, y, z);
        setChildTransforms();
    }
    void scaleBy(glm::vec3 scale) override{
        position->scaleBy(scale);
        setChildTransforms();
    }
    void scaleTo(float x, float y, float z) override{
        position->scaleTo(x, y, z);
        setChildTransforms();
    }
    void scaleTo(glm::vec3 scale) override{
        position->scaleTo(scale);
        setChildTransforms();
    }

    void setChildTransforms() {
        glm::mat4 parentTransform = position->getModelMatrix();
        for (size_t i = 0; i < childs.size(); i++) {
            childs.at(i)->position->setParentTransform(parentTransform);
            childs.at(i)->position->updateMatrices();
            childs.at(i)->setChildTransforms();
        }
    }
    void setDefaultPose() {
        moveTo(0, 0, 0);
        scaleTo(1, 1, 1);
        rotateTo(0, 0, 0);
    }
};

class JointList : public AListO<Joint>, public IShaderField {
public:
    JointList() { name = "jointTransforms"; }

    //TODO refactor body
    void pushToShader(Shader& s) override {
        jointPoses.clear();
        for (size_t i = 0; i < size(); ++i) {
            float* mat =
                (float*)glm::value_ptr(at(i)->position->getModelMatrix());
            for (int j = 0; j < 16; ++j) {
                jointPoses.push_back(mat[j]);
            }
        }
        
        s.setUniformMatrix4fv(name, jointPoses.data(), size());
    }

    // remove?
    void appendFromRootJoint(Joint& j) {
        push(j);
        for (size_t i = 0; i < j.childs.size(); ++i) {
            appendFromRootJoint(*j.childs.at(i));
        }
    }

    Joint& getById(size_t id) {
        for (size_t i = 0; i < size(); ++i) {
            if (at(i)->id == id)
                return *at(i);
        }
        throw string("joint of index range in joint list");
    }
    string genPoseInfo() {
        string res = "p ";
        for (size_t i = 0; i < size(); ++i) {
            res += to_string(at(i)->id) + ' ';
        }
        res += '\n';
        for (size_t i = 0; i < size(); ++i) {
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
    void setDafaultPose() {
        for (size_t i = 0; i < size(); ++i) {
            at(i)->moveTo(0, 0, 0);
            at(i)->scaleTo(1, 1, 1);
            at(i)->rotateTo(0, 0, 0);
        }
    }

private:
    vector<float> jointPoses;
};
