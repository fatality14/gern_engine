#pragma once

#include <common.h>
#include <shaderfield.h>

class IBuffer{};

template <class T, class U>
class ABuffer : public IBuffer{
public:
    ABuffer(T& m, Shader& s, string name = "noname"){
        static_assert(std::is_base_of<AMesh<U>, T>::value, "Template parameter T must be derived from AMesh");
        static_assert(std::is_base_of<IVertex, U>::value, "Template parameter U must be derived from IVertex");

        mesh = &m;
        shader= &s;

        this->name = name;
    }
    ABuffer(ABuffer& b) = delete;
    virtual ~ABuffer(){
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
    }
    void bind(){
        glBindVertexArray(VAO);
    }
    void unbind(){
        glBindVertexArray(0);
    }
    const string& getMeshName(){
        return mesh->name;
    }
    T& getMesh(){
        return *mesh;
    }
    Shader& getShaderPtr(){
        return *shader;
    }
    void setShader(Shader* s){
        shader = s;

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        shaderfields.pushAllToShader(*shader);
        unbind();
    }

    void genBuffers(){
        //init and use vao that works just like vbo and ebo buffers wrapper
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(U) * mesh->vertices->size(), mesh->vertices->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices->size(), mesh->indices->data(), GL_STATIC_DRAW);

        shaderfields.pushAllToShader(*shader);

        //current vao should be set in while window loop
        unbind();
    }

    string name;

protected:
    Shader* shader;
    ShaderFieldList shaderfields;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    T* mesh;

};
