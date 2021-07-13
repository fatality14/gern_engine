#pragma once

#include <common.h>
#include <shader.h>

class LightSource{
public:
    string name;
    glm::vec3 lightPos;

    LightSource(string name = "noname"){
        this->name = name;
        lightPos = glm::vec3(0.f, 0.f, 2.f);
    }

    void pushToShader(Shader* s, string uniformName){
        s->setUniform3fv(uniformName, glm::value_ptr(lightPos));
    }
    void setPosition(float x, float y, float z){
        lightPos.x += x;
        lightPos.y += y;
        lightPos.z += z;
    }
};

class LightSourceList : public List<LightSource>{
public:
    void pushNew(string name = "noname"){
        push(*(new LightSource(name)));
    }
    void pushToShaderByIndex(Shader* s, size_t index, string uniformName){
        LightSource* currentSource = at(index);
        currentSource->pushToShader(s, uniformName);
    }
    void pushToShaderByName(Shader* s, string name, string uniformName){
        LightSource* currentSource = getByName(name);
        s->setUniform3fv(uniformName.data(), glm::value_ptr(currentSource->lightPos));
    }
};
