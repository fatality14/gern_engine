#pragma once

#include "fwd.hpp"
#include <common/common.h>
#include <common/loader.h>
#include <mesh/mesh.h>

class MeshLoader : private ULoader {
public:
    MeshLoader() { allPolyIndexes = new vector<vector<glm::ivec3>>; }

    Mesh* load(const filesystem::path& path, const string& meshName = "noname") {
        mesh = new Mesh(meshName);

        ifstream f;
        f.open(path);

        if (f.fail()) {
            throw string("Cannot open file: ") + path.string();
        }

        string line;

        int switchCase;

        bool nextPart = false;
        bool nextMtl = false;
        GLint lastPolyId = -1;

        while (!f.eof()) {
            switchCase = -1;

            getline(f, line);
            removeExcessSpaces(line);

            token = bite(" ", line);

            if (token == "v") {
                if (nextPart) {
                    lastPolyIds.push_back(lastPolyId);
                    nextPart = false;
                }
                // lastPolyId = -1;
                switchCase = 0;
            }
            if (token == "vn") {
                switchCase = 1;
            }
            if (token == "vt") {
                switchCase = 2;
            }
            if (token == "f") {
                if (nextMtl) {
                    if (lastPolyId != (GLint)-1)
                        lastMtlIds.push_back(lastPolyId);
                    nextMtl = false;
                }
                ++lastPolyId;
                switchCase = 3;
                nextPart = true;
            }
            if (token == "usemtl") {
                switchCase = 4;
                nextMtl = true;
            }
            if (token == "#") {
                switchCase = 5;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0:
                poses.push_back(parseVec3(line));
                break;
            case 1:
                norms.push_back(parseVec3(line));
                break;
            case 2:
                texes.push_back(parseVec2(line));
                break;
            case 3:
                allPolyIndexes->push_back(
                    parseIndexes(line, calcNumArgsDividedBy(" ", line)));
                break;
            case 4:
                // cout << line << endl;
                break;
            }
        }
        if (nextPart == true) {
            lastPolyIds.push_back(lastPolyId);
            lastMtlIds.push_back(lastPolyId);
        }

        dividePolygonsToTriangles();

        GLint startFrom = 0;
        for (size_t i = 0; i < lastPolyIds.size(); ++i) {
            if (mesh->partEndVertexIds.size() > 0)
                mesh->partEndVertexIds.push_back(
                    mesh->partEndVertexIds.at(i - 1));
            else
                mesh->partEndVertexIds.push_back(0);
            for (GLint j = startFrom; j < lastPolyIds.at(i) + 1; ++j) {
                mesh->partEndVertexIds.at(i) += allPolyIndexes->at(j).size();
            }
            startFrom = lastPolyIds.at(i) + 1;
        }

        startFrom = 0;
        for (size_t i = 0; i < lastMtlIds.size(); ++i) {
            if (mesh->partEndMtlIds.size() > 0)
                mesh->partEndMtlIds.push_back(mesh->partEndMtlIds.at(i - 1));
            else
                mesh->partEndMtlIds.push_back(0);
            for (GLint j = startFrom; j < lastMtlIds.at(i) + 1; ++j) {
                mesh->partEndMtlIds.at(i) += allPolyIndexes->at(j).size();
            }
            startFrom = lastMtlIds.at(i) + 1;
        }

        for (auto& currPolyIndexes : *allPolyIndexes) {
            for (auto& indexVec : currPolyIndexes) {
                Vertex v;
                v.position = poses.at(indexVec.x - 1);

                if (texes.size() != 0)
                    v.texcoord = texes.at(indexVec.y - 1);
                else
                    v.texcoord = glm::vec3(0.f);

                if (norms.size() != 0)
                    v.normal = norms.at(indexVec.z - 1);

                mesh->vertices->push_back(v);
                ++mesh->nVertices;
            }
        }

        if (norms.size() == 0) {
            calcNormals();
        }

        clean();

        return mesh;
    }

private:
    string token;

    Mesh* mesh;

    vector<glm::vec3> poses;
    vector<glm::vec3> norms;
    vector<glm::vec2> texes;

    vector<glm::ivec3> indexes;
    vector<vector<glm::ivec3>>* allPolyIndexes;

    vector<GLint> lastPolyIds;
    vector<GLint> lastMtlIds;

    vector<glm::ivec3> parseIndexes(string& line, int numArgsF) {
        GLint ind[3];
        string copyLine = line;
        size_t numInd = calcNumArgsDividedBy("/", bite(" ", copyLine));

        vector<glm::ivec3> indexes;

        // cout << token << " ";
        while (true) {
            if (numInd == 1) {
                token = bite(" ", line);
                ind[0] = stoul(token);
                ind[1] = ind[2] = 1; // very important to set 1
            }
            if (numInd == 2) {
                token = bite("/", line);
                ind[0] = stoul(token);
                token = bite(" ", line);
                if (token != "")
                    ind[1] = stoul(token);
                else
                    ind[1] = 1;
                ind[2] = 1;
            }
            if (numInd == 3) {
                token = bite("/", line);
                ind[0] = stoul(token);

                token = bite("/", line);
                if (token != "")
                    ind[1] = stoul(token);
                else
                    ind[1] = 1;

                token = bite(" ", line);
                if (token != "")
                    ind[2] = stoul(token);
                else
                    ind[2] = 1;
            }

            --numArgsF;
            indexes.push_back(glm::ivec3(ind[0], ind[1], ind[2]));

            if (numArgsF == 0) {
                break;
            }
        }
        return indexes;
    }
    void calcNormals() {
        for (size_t i = 0; i < mesh->vertices->size(); i += 3) {
            glm::vec3 normal;
            normal = glm::normalize(
                glm::cross(mesh->vertices->at(i).position -
                               mesh->vertices->at(i + 1).position,
                           mesh->vertices->at(i + 1).position -
                               mesh->vertices->at(i + 2).position));
            if (glm::isnan(normal.x))
                normal = glm::vec3(0.f);

            mesh->vertices->at(i).normal = normal;
            mesh->vertices->at(i + 1).normal = normal;
            mesh->vertices->at(i + 2).normal = normal;
        }
    }
    void dividePolygonsToTriangles() {
        vector<glm::ivec3> newPolyIndexes;
        vector<vector<glm::ivec3>>* newAllPolyIndexes =
            new vector<vector<glm::ivec3>>;
        size_t i;
        for (auto& currPolyIndexes : *allPolyIndexes) {
            newPolyIndexes.clear();
            if (currPolyIndexes.size() > 3) {
                i = 1;
                for (size_t j = 0; j < currPolyIndexes.size() - 2; j++) {
                    newPolyIndexes.push_back(currPolyIndexes.at(0));
                    newPolyIndexes.push_back(currPolyIndexes.at(i));
                    newPolyIndexes.push_back(currPolyIndexes.at(i + 1));
                    ++i;
                }
                newAllPolyIndexes->push_back(newPolyIndexes);
            } else {
                newAllPolyIndexes->push_back(currPolyIndexes);
            }
        }

        delete allPolyIndexes;
        allPolyIndexes = newAllPolyIndexes;
    }
    void clean() {
        poses.clear();
        norms.clear();
        texes.clear();
        indexes.clear();
        allPolyIndexes->clear();
        lastPolyIds.clear();
        lastMtlIds.clear();
    }
};
