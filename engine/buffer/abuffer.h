#pragma once

#include <common/common.h>
#include <mesh/amesh.h>
#include <mesh/ivertex.h>
#include <shader/ishaderfield.h>
#include <shader/shaderfieldlist.h>

class IBuffer : public ICommon {};

// maybe add BufferList
template <class T, class U> class ABuffer : public IBuffer {
public:
    ABuffer(T& m, Shader& s, const string& name = "noname") {
        static_assert(std::is_base_of<IMesh, T>::value,
                      "Template parameter T must be derived from IMesh");
        static_assert(std::is_base_of<IVertex, U>::value,
                      "Template parameter U must be derived from IVertex");

        mesh = &m;
        shader = &s;

        this->name = name;
    }
    ABuffer(ABuffer& b) = delete;
    virtual ~ABuffer() {
        GLDB(glDeleteBuffers(1, &VBO));
        GLDB(glDeleteVertexArrays(1, &VAO));
    }
    void bind() { GLDB(glBindVertexArray(VAO)); }
    void unbind() { glBindVertexArray(0); }
    const string& getMeshName() { return mesh->name; }
    T& getMesh() { return *mesh; }
    Shader& getShaderPtr() { return *shader; }
    void setShader(Shader& s) {
        shader = &s;

        GLDB(glBindVertexArray(VAO));
        GLDB(glBindBuffer(GL_ARRAY_BUFFER, VBO));

        shaderfields.pushToShader(*shader);
        unbind();
    }

    void genBuffers() {
        GLDB(glCreateVertexArrays(1, &VAO));
        GLDB(glBindVertexArray(VAO));

        GLDB(glGenBuffers(1, &VBO));
        GLDB(glBindBuffer(GL_ARRAY_BUFFER, VBO));
        GLDB(glBufferData(GL_ARRAY_BUFFER, sizeof(U) * mesh->vertices->size(),
                          mesh->vertices->data(), GL_STATIC_DRAW));

        shaderfields.pushToShader(*shader);

        // current vao should be set in while window loop
        unbind();
    }

    string name;

protected:
    Shader* shader;
    ShaderFieldList shaderfields;

    GLuint VAO;
    GLuint VBO;

    T* mesh;
};
