#pragma once

#include <mesh/skeletonmesh.h>
#include <buffer/abuffer.h>
#include <shader/shaderattrib.h>

class SkeletonBuffer : public ABuffer<SkeletonMesh, SkeletonVertex>{
public:
    SkeletonBuffer(SkeletonMesh& m, Shader& s, string name = "noname") : ABuffer(m, s, name){
        shaderfields.push(vpos);
        shaderfields.push(vtex);
        shaderfields.push(vnorm);
        shaderfields.push(vjoints);
        shaderfields.push(vweights);
    }
private:
    ShaderAttrib<GLfloat, SkeletonVertex> vpos = ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_position", 3,
                offsetof(SkeletonVertex, position));

    ShaderAttrib<GLfloat, SkeletonVertex> vtex = ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_texcoord", 2,
                offsetof(SkeletonVertex, texcoord));

    ShaderAttrib<GLfloat, SkeletonVertex> vnorm = ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_normal", 3,
                offsetof(SkeletonVertex, normal));
    ShaderAttrib<GLfloat, SkeletonVertex> vjoints = ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_joints", 3,
                offsetof(SkeletonVertex, joints));
    ShaderAttrib<GLfloat, SkeletonVertex> vweights = ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_weights", 3,
                offsetof(SkeletonVertex, weights));
};
