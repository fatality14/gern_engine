#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/skeletonmesh.h>

class Skeletizer{
public:
    SkeletonMesh& skeletize(Mesh& mesh, Mesh& skeleton){
        skeletonMesh = new SkeletonMesh;
        skeletonMesh->name = mesh.name;

        skeletonMesh->nIndices = mesh.nIndices;
        skeletonMesh->nVertices = mesh.nVertices;
        skeletonMesh->partEndMtlIds = mesh.partEndMtlIds;
        skeletonMesh->partEndVertexIds = mesh.partEndVertexIds;
        skeletonMesh->joints.appendEmptyJoints(50);
        skeletonMesh->joints.at(0)->rotate(30,0,0);

        for(size_t i = 0; i < mesh.nVertices; i++){
            SkeletonVertex v;
            v.position = mesh.vertices->at(i).position;
            v.normal = mesh.vertices->at(i).normal;
            v.texcoord = mesh.vertices->at(i).texcoord;
            v.joints = glm::ivec3(0);
            v.weights = glm::vec3(1,0,0);

            skeletonMesh->pushVertex(v);
        }

        return *skeletonMesh;
    }

private:
    SkeletonMesh* skeletonMesh;
};
