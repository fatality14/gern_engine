#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/shader.h>

//maybe add BufferList
class Buffer{
public:
    Buffer(Mesh& m, Shader& s){
        mesh = &m;
        shader= &s;

        genBuffers();
    }
    Buffer(Buffer& b) = delete;
    ~Buffer(){
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
    Mesh& getMesh(){
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

        shader->setVertexAttribPointer("vertex_position", 3, offsetof(Vertex, position));
        shader->setVertexAttribPointer("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setVertexAttribPointer("vertex_normal", 3, offsetof(Vertex, normal));

        unbind();
    }

private:
    Shader* shader;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    Mesh* mesh;

    void genBuffers(){
        //init and use vao that works just like vbo and ebo buffers wrapper
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->vertices->size(), mesh->vertices->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices->size(), mesh->indices->data(), GL_STATIC_DRAW);

        shader->setVertexAttribPointer("vertex_position", 3, offsetof(Vertex, position));
        shader->setVertexAttribPointer("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setVertexAttribPointer("vertex_normal", 3, offsetof(Vertex, normal));

        //current vao should be set in while window loop
        unbind();
    }
};
