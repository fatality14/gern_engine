#pragma once

#include <skeletonmesh.h>
#include <abuffer.h>

//maybe add BufferList
class SkeletonBuffer : public ABuffer<SkeletonMesh, SkeletonVertex>{
public:
    SkeletonBuffer(SkeletonMesh& m, Shader& s, string name = "noname") : ABuffer(m, s, name){
        shaderfields.push(*vpos);
        shaderfields.push(*vtex);
        shaderfields.push(*vnorm);
        shaderfields.push(*vjoints);
        shaderfields.push(*vweights);
    }
    ~SkeletonBuffer(){
        delete vpos;
        delete vtex;
        delete vnorm;
        delete vjoints;
        delete vweights;
    }

    IShaderField* vpos = new ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_position", 3,
                offsetof(SkeletonVertex, position));

    IShaderField* vtex = new ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_texcoord", 2,
                offsetof(SkeletonVertex, texcoord));

    IShaderField* vnorm = new ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_normal", 3,
                offsetof(SkeletonVertex, normal));
    IShaderField* vjoints = new ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_joints", 3,
                offsetof(SkeletonVertex, joints));
    IShaderField* vweights = new ShaderAttrib<GLfloat, SkeletonVertex>(
                "vertex_weights", 3,
                offsetof(SkeletonVertex, weights));
};
