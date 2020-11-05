#pragma once

#include <engine/common.h>
#include <engine/mesh.h>
#include <engine/skeletonmesh.h>

class Skeletizer : private Loader{
public:
    SkeletonMesh& skeletize(Mesh& mesh, string skeletonPath){
        skeletonMesh = new SkeletonMesh;
        skeletonMesh->name = mesh.name;

        skeletonMesh->nIndices = mesh.nIndices;
        skeletonMesh->nVertices = mesh.nVertices;
        skeletonMesh->partEndMtlIds = mesh.partEndMtlIds;
        skeletonMesh->partEndVertexIds = mesh.partEndVertexIds;
//        skeletonMesh->joints.appendEmptyJoints(50);
//        skeletonMesh->joints.at(0)->rotate(30,0,0);

        for(size_t i = 0; i < mesh.nVertices; i++){
            SkeletonVertex v;
            v.position = mesh.vertices->at(i).position;
            v.normal = mesh.vertices->at(i).normal;
            v.texcoord = mesh.vertices->at(i).texcoord;
            v.joints = glm::ivec3(0);
            v.weights = glm::vec3(1,0,0);

            skeletonMesh->pushVertex(v);
        }

        parseSkeleton(skeletonPath);

        return *skeletonMesh;
    }

private:
    SkeletonMesh* skeletonMesh;

    void parseSkeleton(string filePath){
        ifstream f;
        f.open(filePath);

        string line;
        int switchCase;
        bool end;
        vector<pair<int, int>> childTree;
        size_t jAmount = -1;

        while(!f.eof()){
            switchCase = -1;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "j"){
                switchCase = 0;
            }
            if(token == "v1"){
                switchCase = 1;
            }
            if(token == "v2"){
                switchCase = 2;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0: {
                Joint* j = new Joint;
                int i = -1;
                bool end;
                while (true){
                    ++i;
                    token = bite(" ", line, end);
                    if(i == 0)
                        j->name = token;
                    if(i == 1)
                        j->id = stoi(token);
                    if(i >= 2){
                        if(token != "")
                            childTree.push_back(make_pair(j->id, stoi(token)));
                    }
                    if(end)
                        break;
                }
                ++jAmount;
                skeletonMesh->joints.push(*j);
                break;
            }
            case 1:
                skeletonMesh->joints.at(jAmount)->firstPos = parseVec3(line);
                break;
            case 2:
                skeletonMesh->joints.at(jAmount)->secondPos = parseVec3(line);
                break;
            }
        }

        for(size_t i = 0; i < childTree.size(); i++){
            skeletonMesh->joints.getById(childTree.at(i).first).childs.push_back(&skeletonMesh->joints.getById(childTree.at(i).second));
        }
    }
};
