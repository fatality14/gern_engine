#pragma once

#include <common.h>
#include <shader.h>
#include <type_traits>

class IShaderField : public ICommon{
public:
    virtual void pushToShader(Shader& shader) = 0;
    virtual ~IShaderField() = default;
    string name;
    bool selfPushable = true;
};

template <class T>
class ShaderUniform : public IShaderField{
public:
    ShaderUniform(T value, string uniformName, size_t size = 1){
        this->value = value;
        this->name = uniformName;
        this->size = size;
    }

    void pushToShader(Shader& shader) override{
        //works on c++17, for c++14, c++11 need to use "std::enable_if_t"
        if constexpr (std::is_same<T, glm::mat4>::value){
            shader.setUniformMatrix4fv(name, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, glm::vec3>::value){
            shader.setUniform3fv(name, glm::value_ptr(value), size);
        }
        else if constexpr (std::is_same<T, GLint>::value){
            shader.setUniform1i(name, value);
        }
        else if constexpr (std::is_same<T, GLfloat>::value){
            shader.setUniform1f(name, value);
        }
    }

    void setValue(T value){
        this->value = value;
    }
private:
    T value;
    size_t size;
};

//consider add VBO/VAO id to avoid manual VBO/VAO binding
//note that buffer data must be added manualy cause of idea of buffer data
template <class T, class U>
class ShaderAttrib : public IShaderField{
public:
    ShaderAttrib(string layoutName, int vecSize, size_t offset){
        this->name = layoutName;
        this->vecSize = vecSize;
        this->offset = offset;
    }

    void pushToShader(Shader& shader) override{
        if constexpr (std::is_same<T, GLint>::value){
            shader.setIntAttribPointer<U>(name, vecSize, offset);
        }
        else if constexpr (std::is_same<T, GLfloat>::value){
            shader.setFloatAttribPointer<U>(name, vecSize, offset);
        }
    }
private:
    int vecSize;
    size_t offset;
};

template <class U>
class ShaderMatAttrib : public IShaderField{
public:
    ShaderMatAttrib(string layoutName, int matSize, size_t offset, size_t divisor = 0){
        this->name = layoutName;
        this->matSize = matSize;
        this->offset = offset;
        this->divisor = divisor;
    }

    void pushToShader(Shader& shader) override{
        shader.setMatAttribPointer<U>(name, matSize, offset, divisor);
    }
private:
    int matSize;
    size_t offset;
    size_t divisor;
};

class ShaderFieldList : public AList<IShaderField>{
public:
    ShaderFieldList(){
        owndestructor = true;
    }
    ~ShaderFieldList(){

    }

    void pushToShader(Shader& shader){
        for(size_t i = 0; i < size(); ++i){
            if(at(i)->selfPushable){
                at(i)->pushToShader(shader);
            }
        }
    }
};
