#pragma once

#include <common.h>
#include <mesh.h>
#include <shader.h>
#include <skeletonmesh.h>

//maybe add BufferList
class SkeletonBuffer{
public:
    SkeletonBuffer(SkeletonMesh& m, Shader& s, string name = "noname"){
        mesh = &m;
        shader= &s;

        this->name = name;

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

        shader->setFloatAttribPointer<SkeletonVertex>("vertex_position", 3, offsetof(SkeletonVertex, position));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_texcoord", 2, offsetof(SkeletonVertex, texcoord));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_normal", 3, offsetof(SkeletonVertex, normal));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_joints", 3, offsetof(SkeletonVertex, joints));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_weights", 3, offsetof(SkeletonVertex, weights));

        unbind();
    }

    string name;
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

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(SkeletonVertex) * mesh->vertices->size(), mesh->vertices->data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices->size(), mesh->indices->data(), GL_STATIC_DRAW);

        shader->setFloatAttribPointer<SkeletonVertex>("vertex_position", 3, offsetof(SkeletonVertex, position));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_texcoord", 2, offsetof(SkeletonVertex, texcoord));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_normal", 3, offsetof(SkeletonVertex, normal));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_joints", 3, offsetof(SkeletonVertex, joints));
        shader->setFloatAttribPointer<SkeletonVertex>("vertex_weights", 3, offsetof(SkeletonVertex, weights));

        //current vao should be set in while window loop
        unbind();
    }
};
