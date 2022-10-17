#pragma once

#include <buffer/abuffer.h>
#include <mesh/mesh.h>
#include <shader/shaderattrib.h>

class MeshBuffer : public ABuffer<Mesh, Vertex> {
public:
    MeshBuffer(Mesh& m, Shader& s, string name = "noname")
        : ABuffer(m, s, name) {
        shaderfields.push(vpos);
        shaderfields.push(vtex);
        shaderfields.push(vnorm);
    }

private:
    ShaderAttrib<GLfloat, Vertex> vpos = ShaderAttrib<GLfloat, Vertex>(
        "vertex_position", 3, offsetof(Vertex, position));

    ShaderAttrib<GLfloat, Vertex> vtex = ShaderAttrib<GLfloat, Vertex>(
        "vertex_texcoord", 2, offsetof(Vertex, texcoord));

    ShaderAttrib<GLfloat, Vertex> vnorm = ShaderAttrib<GLfloat, Vertex>(
        "vertex_normal", 3, offsetof(Vertex, normal));
};
