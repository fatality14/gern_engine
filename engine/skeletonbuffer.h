#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/shader.h>
#include <engine/skeletonmesh.h>

//maybe add BufferList
class SkeletonBuffer{
public:
    SkeletonBuffer(SkeletonMesh& m, Shader& s){
        mesh = &m;
        shader= &s;

        genBuffers();
    }
    SkeletonBuffer(SkeletonBuffer& b) = delete;
    ~SkeletonBuffer(){
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
    SkeletonMesh& getMesh(){
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

        shader->setSkeletonVertexAttribPointer("vertex_position", 3, offsetof(SkeletonVertex, position));
        shader->setSkeletonVertexAttribPointer("vertex_texcoord", 2, offsetof(SkeletonVertex, texcoord));
        shader->setSkeletonVertexAttribPointer("vertex_normal", 3, offsetof(SkeletonVertex, normal));
        shader->setSkeletonVertexIntAttribPointer("vertex_joints", 3, offsetof(SkeletonVertex, joints));
        shader->setSkeletonVertexAttribPointer("vertex_weights", 3, offsetof(SkeletonVertex, weights));

        unbind();
    }

private:
    Shader* shader;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    SkeletonMesh* mesh;

    void genBuffers(){
        //init and use vao that works just like vbo and ebo buffers wrapper
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        //vbo push all vertices array data to gpu and interpret it like some data array
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkeletonVertex) * mesh->vertices->size(), mesh->vertices->data(), GL_STATIC_DRAW);

        //ebo push all indices array data to gpu and interpret it like element array
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices->size(), mesh->indices->data(), GL_STATIC_DRAW);

        shader->setSkeletonVertexAttribPointer("vertex_position", 3, offsetof(SkeletonVertex, position));
        shader->setSkeletonVertexAttribPointer("vertex_texcoord", 2, offsetof(SkeletonVertex, texcoord));
        shader->setSkeletonVertexAttribPointer("vertex_normal", 3, offsetof(SkeletonVertex, normal));
        shader->setSkeletonVertexIntAttribPointer("vertex_joints", 3, offsetof(SkeletonVertex, joints));
        shader->setSkeletonVertexAttribPointer("vertex_weights", 3, offsetof(SkeletonVertex, weights));

        //current vao should be set in while window loop
        unbind();
    }
};
