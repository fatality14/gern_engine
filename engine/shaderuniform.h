#pragma once

#include <engine/common.h>
#include <engine/shader.h>
#include <type_traits>

class ShaderUniformBaseClass{
public:
    virtual void pushToShader() = 0;
};

template <class T>
class ShaderUniform : public ShaderUniformBaseClass{
public:
    ShaderUniform(T value, string uniformName, Shader* s, size_t size = 1){
        this->value = value;
        this->uniformName = uniformName;
        this->shader = s;
        this->size = size;
    }

    void pushToShader() override{
        //works on c++17, for c++14,c++11 need to use "std::enable_if_t"
        if constexpr (std::is_same<T, glm::mat4>::value){
            shader->setUniformMatrix4fv(uniformName, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, glm::vec3>::value){
            shader->setUniform3fv(uniformName, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, GLint>::value){
            shader->setUniform1i(uniformName, value);
        }
        else if constexpr (std::is_same<T, GLfloat>::value){
            shader->setUniform1f(uniformName, value);
        }
    }
private:
    T value;
    string uniformName;
    Shader* shader;
    size_t size;
};

class ShaderUniformList : public List<ShaderUniformBaseClass>{
public:
    void pushToShaders(){
        for(size_t i = 0; i < size(); ++i){
            at(i)->pushToShader();
        }
    }
};
