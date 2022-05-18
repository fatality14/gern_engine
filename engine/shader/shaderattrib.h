#pragma once

#include <shader/shaderfield.h>

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
