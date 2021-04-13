#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/shader.h>
#include <engine/position.h>

//maybe add BufferList
class InstancedBuffer{
public:
    InstancedBuffer(Mesh& m, Shader& s){
        mesh = &m;
        shader= &s;

        genBuffers();
    }
    InstancedBuffer(InstancedBuffer& b) = delete;
    ~InstancedBuffer(){
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &VBO1);
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
    void setModelMatrices(vector<glm::mat4>& modelMatrices){
        this->modelMatrices = modelMatrices;

        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO1);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices.size(), modelMatrices.data(), GL_STATIC_DRAW);

        shader->setMatAttribPointer<glm::mat4>("model", 4, 0, 1);
    }
    void setShader(Shader* s){
        shader = s;

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        shader->setFloatAttribPointer<Vertex>("vertex_position", 3, offsetof(Vertex, position));
        shader->setFloatAttribPointer<Vertex>("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setFloatAttribPointer<Vertex>("vertex_normal", 3, offsetof(Vertex, normal));

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);

        shader->setMatAttribPointer<glm::mat4>("model", 4, 0, 1);

        unbind();
    }

private:
    Shader* shader;

    GLuint VAO;
    GLuint VBO, VBO1;
    GLuint EBO;

    Mesh* mesh;

    vector<glm::mat4> modelMatrices;

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

        shader->setFloatAttribPointer<Vertex>("vertex_position", 3, offsetof(Vertex, position));
        shader->setFloatAttribPointer<Vertex>("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setFloatAttribPointer<Vertex>("vertex_normal", 3, offsetof(Vertex, normal));

        //current vao should be set in while window loop
        unbind();
    }
};
