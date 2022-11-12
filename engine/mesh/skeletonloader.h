#pragma once

#include <algorithm>
#include <common/common.h>
#include <common/loader.h>
#include <filesystem>
#include <mesh/mesh.h>
#include <mesh/skeletonmesh.h>

class SkeletonLoader : private ULoader {
public:
    SkeletonMesh& skeletize(Mesh& mesh, const filesystem::path& skeletonPath) {
        skeletonMesh = new SkeletonMesh;
        skeletonMesh->name = mesh.name;

        skeletonMesh->nVertices = mesh.nVertices;
        skeletonMesh->partEndMtlIds = mesh.partEndMtlIds;
        skeletonMesh->partEndVertexIds = mesh.partEndVertexIds;

        parseSkeleton(skeletonPath);
        addWeights(mesh);

        return *skeletonMesh;
    }

private:
    SkeletonMesh* skeletonMesh;

    void parseSkeleton(const filesystem::path& filePath) {
        ifstream f;
        f.open(filePath);

        if (f.fail()) {
            throw string("Cannot open file: ") + filePath.string();
        }

        string line;
        int switchCase;
        vector<pair<int, int>> childTree;
        size_t jAmount = -1;

        string token;

        while (!f.eof()) {
            switchCase = -1;

            getline(f, line);
            removeExcessSpaces(line);

            token = bite(" ", line);

            if (token == "j") {
                switchCase = 0;
            }
            if (token == "vt") {
                switchCase = 1;
            }
            if (token == "vo") {
                switchCase = 2;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0: {
                Joint* j = new Joint;
                int i = -1;
                bool end;
                while (true) {
                    ++i;
                    token = bite(" ", line, end);
                    if (i == 0)
                        j->name = token;
                    if (i == 1)
                        j->id = stoi(token);
                    if (i >= 2) {
                        if (token != "")
                            childTree.push_back(make_pair(j->id, stoi(token)));
                    }
                    if (end)
                        break;
                }
                ++jAmount;
                skeletonMesh->joints.push(*j);
                break;
            }
            case 1:
                skeletonMesh->joints.at(jAmount)->position->tip =
                    parseVec3(line);
                break;
            case 2:
                skeletonMesh->joints.at(jAmount)->position->origin =
                    parseVec3(line);
                break;
            }
        }

        for (size_t i = 0; i < childTree.size(); i++) {
            skeletonMesh->joints.getById(childTree.at(i).first)
                .childs.push_back(
                    &skeletonMesh->joints.getById(childTree.at(i).second));
        }
    }
    void addWeights(Mesh& mesh) {
        for (GLint i = 0; i < mesh.nVertices; i++) {
            SkeletonVertex v;
            v.position = mesh.vertices->at(i).position;
            v.normal = mesh.vertices->at(i).normal;
            v.texcoord = mesh.vertices->at(i).texcoord;
            v.joints = glm::vec3(0);
            v.weights = glm::vec3(0);

            vector<float> ds;
            for (size_t i = 0; i < skeletonMesh->joints.size(); ++i) {
                glm::vec3 closest = glm::closestPointOnLine(
                    v.position, skeletonMesh->joints.at(i)->position->tip,
                    skeletonMesh->joints.at(i)->position->origin);
                ds.push_back(glm::length(v.position - closest));
            }

            vector<float> sorted = ds;
            sort(sorted.begin(), sorted.end());

            for (size_t i = 0; i < skeletonMesh->joints.size(); ++i) {
                if (ds.at(i) == sorted.at(0)) {
                    v.joints[0] = i;
                    break;
                }
            }
            for (size_t i = 0; i < skeletonMesh->joints.size(); ++i) {
                if (ds.at(i) == sorted.at(1)) {
                    v.joints[1] = i;
                    break;
                }
            }
            for (size_t i = 0; i < skeletonMesh->joints.size(); ++i) {
                if (ds.at(i) == sorted.at(2)) {
                    v.joints[2] = i;
                    break;
                }
            }

            float d1 = sorted.at(0);
            float d2 = sorted.at(1);
            float d3 = sorted.at(2);
            float l = pow(1 / d1, 4) + pow(1 / d2, 4) + pow(1 / d3, 4);

            v.weights.x = pow(1 / d1, 4) / l;
            v.weights.y = pow(1 / d2, 4) / l;
            v.weights.z = pow(1 / d3, 4) / l;

            skeletonMesh->pushVertex(v);
        }
    }
};
