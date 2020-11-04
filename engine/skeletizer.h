#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/skeletonmesh.h>

class Skeletizer{
public:
    SkeletonMesh& skeletize(Mesh& mesh, Mesh& skeleton){
        skeletonMesh = new SkeletonMesh();
        skeletonMesh->name = mesh.name;

        SkeletonVertex* v;
        for(size_t i = 0; i < mesh.nVertices; i++){
            v = new SkeletonVertex;
            v->position = mesh.vertices->at(i).position;
            v->normal = mesh.vertices->at(i).normal;
            v->texcoord = mesh.vertices->at(i).texcoord;
            v->joints = glm::vec3(0);
            v->weights = glm::vec3(0);

            skeletonMesh->pushVertex(*v);
        }

        return *skeletonMesh;
    }

    SkeletonMesh* skeletonMesh;
};
