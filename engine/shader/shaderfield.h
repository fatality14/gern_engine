#pragma once

#include <common/common.h>
#include <shader/shader.h>
#include <type_traits>

class IShaderField : public ICommon{
public:
    virtual void pushToShader(Shader& shader) = 0;
    virtual ~IShaderField() = default;
    string name;
    bool selfPushable = true;
};
