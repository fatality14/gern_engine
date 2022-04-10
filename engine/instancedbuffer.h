#pragma once

#include <mesh.h>
#include <position.h>
#include <abuffer.h>

//maybe add BufferList
class InstancedBuffer : public ABuffer<Mesh, Vertex, InstancedBuffer>{
public:
    using ABuffer::ABuffer;

    ~InstancedBuffer(){
        glDeleteBuffers(1, &VBO1);
    }
    void setModelMatrices(vector<Position>& modelMatrices){
        this->modelMatrices.clear();
        for(size_t i = 0; i < modelMatrices.size(); ++i){
            this->modelMatrices.push_back(modelMatrices[i].getModelMatrix());
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * this->modelMatrices.size(), this->modelMatrices.data(), GL_STATIC_DRAW);

        shader->setMatAttribPointer<glm::mat4>("model", 4, 0, 1);
    }
    void setShader(Shader* s){
        ABuffer::setShader(s);

        bind();

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);

        shader->setMatAttribPointer<glm::mat4>("model", 4, 0, 1);

        unbind();
    }

    static void specifyAttribPointers(Shader* s){
        s->setFloatAttribPointer<Vertex>("vertex_position", 3, offsetof(Vertex, position));
        s->setFloatAttribPointer<Vertex>("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        s->setFloatAttribPointer<Vertex>("vertex_normal", 3, offsetof(Vertex, normal));
    }

    void genBuffers(){
        ABuffer::genBuffers();

        bind();

        glGenBuffers(1, &VBO1);

        //current vao should be set in while window loop
        unbind();
    }

private:
    GLuint VBO1;
    vector<glm::mat4> modelMatrices;
};
