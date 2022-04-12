#pragma once

#include <mesh.h>
#include <abuffer.h>

//maybe add BufferList
class Buffer : public ABuffer<Mesh, Vertex>{
public:
    Buffer(Mesh& m, Shader& s, string name = "noname") : ABuffer(m, s, name){
        shaderfields.push(*vpos);
        shaderfields.push(*vtex);
        shaderfields.push(*vnorm);
    }
    ~Buffer(){
        delete vpos;
        delete vtex;
        delete vnorm;
    }
    IShaderField* vpos = new ShaderAttrib<GLfloat, Vertex>(
                "vertex_position", 3,
                offsetof(Vertex, position));

    IShaderField* vtex = new ShaderAttrib<GLfloat, Vertex>(
                "vertex_texcoord", 2,
                offsetof(Vertex, texcoord));

    IShaderField* vnorm = new ShaderAttrib<GLfloat, Vertex>(
                "vertex_normal", 3,
                offsetof(Vertex, normal));
};
