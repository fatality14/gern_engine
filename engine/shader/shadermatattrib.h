#pragma once

#include <shader/shaderfield.h>

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
