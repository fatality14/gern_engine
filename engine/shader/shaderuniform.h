#pragma once

#include <shader/ishaderfield.h>

template <class T> class ShaderUniform : public IShaderField {
public:
    ShaderUniform(T value, string uniformName, size_t size = 1) {
        this->value = value;
        this->name = uniformName;
        this->size = size;
    }

    void pushToShader(Shader& shader) override {
        // works on c++17, for c++14, c++11 need to use "std::enable_if_t"
        if constexpr (std::is_same<T, glm::mat4>::value) {
            shader.setUniformMatrix4fv(name, glm::value_ptr(value), size);
        } else if constexpr (std::is_same<T, glm::vec3>::value) {
            shader.setUniform3fv(name, glm::value_ptr(value), size);
        } else if constexpr (std::is_same<T, GLint>::value) {
            shader.setUniform1i(name, value);
        } else if constexpr (std::is_same<T, GLfloat>::value) {
            shader.setUniform1f(name, value);
        }
    }

    void setValue(T value) { this->value = value; }

private:
    T value;
    size_t size;
};
