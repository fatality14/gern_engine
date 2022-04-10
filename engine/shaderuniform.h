#pragma once

#include <common.h>
#include <shader.h>
#include <type_traits>

class ShaderUniformBaseClass{
public:
    virtual void pushToShader(Shader* shader) = 0;
};

template <class T>
class ShaderUniform : public ShaderUniformBaseClass{
public:
    ShaderUniform(T value, string uniformName, Shader* s, size_t size = 1){
        this->value = value;
        this->name = uniformName;
        this->shader = s;
        this->size = size;
    }

    void pushToShader(Shader* shader) override{
        //works on c++17, for c++14,c++11 need to use "std::enable_if_t"
        if constexpr (std::is_same<T, glm::mat4>::value){
            shader->setUniformMatrix4fv(name, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, glm::vec3>::value){
            shader->setUniform3fv(name, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, GLint>::value){
            shader->setUniform1i(name, value);
        }
        else if constexpr (std::is_same<T, GLfloat>::value){
            shader->setUniform1f(name, value);
        }
    }

    void setValue(T value){
        this->value = value;
    }
private:
    T value;
    string name;
    size_t size;
};

class ShaderUniformList : public AList<ShaderUniformBaseClass>{
public:
    void pushAllToShader(Shader* shader){
        for(size_t i = 0; i < size(); ++i){
            at(i)->pushToShader(shader);
        }
    }
};
