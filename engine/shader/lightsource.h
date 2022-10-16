#pragma once

#include <common/common.h>
#include <shader/ishaderfield.h>

class LightSource : public IShaderField{
public:
    glm::vec3 lightPos;

    LightSource(string name = "noname"){
        this->name = name;
        lightPos = glm::vec3(0.f, 0.f, 2.f);
    }

    void pushToShader(Shader& s) override{
        s.setUniform3fv(name, glm::value_ptr(lightPos));
    }
    void setPosition(float x, float y, float z){
        lightPos.x += x;
        lightPos.y += y;
        lightPos.z += z;
    }
};

class LightSourceList : public AListO<LightSource>, public IShaderField{
public:
    void pushNew(string name = "noname"){
        push(*(new LightSource(name)));
    }
    void pushToShader(Shader& shader) override{
        for(size_t i = 0; i < size(); ++i){
            list.at(i)->pushToShader(shader);
        }
    }
};
