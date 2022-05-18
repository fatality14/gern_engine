#pragma once

#include <shader/shaderfield.h>

class ShaderFieldList : public AList<IShaderField>{
public:
    void pushToShader(Shader& shader){
        for(size_t i = 0; i < size(); ++i){
            if(at(i)->selfPushable){
                at(i)->pushToShader(shader);
            }
        }
    }
};
