#pragma once

#include <engine/common.h>
#include <engine/mesh.h>

class MeshLoader{
public:
    MeshLoader(){
        allPolyIndexes = new vector<vector<intvec3>>;
    }

    Mesh& load(string path, string meshName = "noname"){
        mesh = new Mesh(meshName);

        ifstream f;
        f.open(path);

        string line;

        int switchCase;
        bool end;

        bool nextPart = false;
        int lastPolyId = -1;

        while(!f.eof()){
            switchCase = -1;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "v"){
                if(nextPart){
                    lastPolyIds.push_back(lastPolyId);
                    nextPart = false;
                }
                //lastPolyId = -1;
                switchCase = 0;
            }
            if(token == "vn"){
                switchCase = 1;
            }
            if(token == "vt"){
                switchCase = 2;
            }
            if(token == "f"){
                ++lastPolyId;
                switchCase = 3;
                nextPart = true;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0:
                poses.push_back(parsePos(line));
                break;
            case 1:
                norms.push_back(parseNorm(line));
                break;
            case 2:
                texes.push_back(parseNorm(line));
                break;
            case 3:
                allPolyIndexes->push_back(parseIndexes(line, calcNumArgsDividedBy(" ", line)));
                break;
            }
        }
        if(nextPart == true){
            lastPolyIds.push_back(lastPolyId);
        }

        dividePolygonsToTriangles();

        int startFrom = 0;
        for(size_t i = 0; i < lastPolyIds.size(); ++i){
            if(mesh->partEndVertexIds.size() > 0)
                mesh->partEndVertexIds.push_back(mesh->partEndVertexIds.at(i-1));
            else
                mesh->partEndVertexIds.push_back(0);
            for(int j = startFrom; j < lastPolyIds.at(i)+1; ++j){
                    mesh->partEndVertexIds.at(i) += allPolyIndexes->at(j).size();
            }
            startFrom = lastPolyIds.at(i)+1;
        }

        for(auto& currPolyIndexes : *allPolyIndexes){
            for(auto& indexVec : currPolyIndexes){
                Vertex v;
                v.position = poses.at(indexVec.x - 1);

                if(texes.size() != 0)
                    v.texcoord = texes.at(indexVec.y - 1);
                else
                    v.texcoord = glm::vec3(0.f);

                if(norms.size() != 0)
                    v.normal = norms.at(indexVec.z - 1);

                v.color = glm::vec3(0.f);

                mesh->vertices->push_back(v);
                ++mesh->nVertices;
            }
        }

        mesh->nIndices = 0;

        if(norms.size() == 0){
            calcNormals();
        }

        clean();

        return *mesh;
    }
private:
    Mesh* mesh;

    string token;

    vector<glm::vec3> poses;
    vector<glm::vec3> norms;
    vector<glm::vec2> texes;

    vector<intvec3> indexes;
    vector<vector<intvec3>>* allPolyIndexes;

    vector<int> lastPolyIds;

    void removeBadSpaces(string& line){
        size_t i = -1;
        while(true){
            ++i;
            if(i >= line.size() - 1 || line.size() < 2)
                break;
            if(line.at(i) == line.at(i+1) && line.at(i) == ' '){
                line.erase(line.begin()+i);
                --i;
            }
        }
        if(line.size() == 0)
            return;

        while(true){
            if(line.at(line.size()-1) == ' ')
                line.erase(line.begin() + line.size()-1);
            else
                break;
        }
        while(true){
            if(line.at(0) == ' ')
                line.erase(line.begin());
            else
                break;
        }
    }
    string& bite(const string& delimiter, string& line, bool& end){
        size_t pos = 0;
        string* token = new string;

        pos = line.find(delimiter);
        end = false;

        if(pos != string::npos){
            *token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
        }
        else{
            end = true;
            token = &line;
        }
        return *token;
    }
    glm::vec3& parsePos(string& line){
        float pos[3];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 3)
                pos[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec3(pos[0], pos[1], pos[2]);
    }
    glm::vec3& parseNorm(string& line){
        float norm[3];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 3)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec3(norm[0], norm[1], norm[2]);
    }
    glm::vec2& parseTex(string& line){
        float norm[2];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 2)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec2(norm[0], norm[1]);
    }
    vector<intvec3>& parseIndexes(string& line, int numArgsF){
        int ind[3];
        string copyLine = line;
        bool end;
        int numInd = calcNumArgsDividedBy("/", bite(" ", copyLine, end));

        vector<intvec3>* indexes = new vector<intvec3>;

        //cout << token << " ";
        while (true){
            if(numInd == 1){
                token = bite(" ", line, end);
                ind[0] = stoi(token);
                ind[1] = ind[2] = 1;//very important to set 1
            }
            if(numInd == 2){
                token = bite("/", line, end);
                ind[0] = stoi(token);
                token = bite(" ", line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;
                ind[2] = 1;
            }
            if(numInd == 3){
                token = bite("/", line, end);
                ind[0] = stoi(token);

                token = bite("/", line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;

                token = bite(" ", line, end);
                if(token != "")
                    ind[2] = stoi(token);
                else
                    ind[2] = 1;
            }

            --numArgsF;
            indexes->push_back(intvec3(ind[0], ind[1], ind[2]));

            if(numArgsF == 0){
                break;
            }
        }
        return *indexes;
    }
    int calcNumArgsDividedBy(string delimiter, string line){
        int numArgs = 0;
        string token;
        bool end;

        while(true){
            token = bite(delimiter, line, end);
            ++numArgs;
            if(end)
                break;
        }
        return numArgs;
    }
    void calcNormals(){
        for(size_t i = 0; i < mesh->vertices->size(); i += 3){
            glm::vec3 normal;
            normal = glm::normalize(glm::cross(mesh->vertices->at(i).position - mesh->vertices->at(i+1).position,
                                               mesh->vertices->at(i+1).position - mesh->vertices->at(i+2).position));
            if(glm::isnan(normal.x))
                normal = glm::vec3(0.f);

            mesh->vertices->at(i).normal = normal;
            mesh->vertices->at(i+1).normal = normal;
            mesh->vertices->at(i+2).normal = normal;
        }
    }
    void dividePolygonsToTriangles(){
        vector<intvec3> newPolyIndexes;
        vector<vector<intvec3>>* newAllPolyIndexes = new vector<vector<intvec3>>;
        int i;
        for(auto& currPolyIndexes : *allPolyIndexes){
            newPolyIndexes.clear();
            if(currPolyIndexes.size() > 3){
                i = 1;
                for(size_t j = 0; j < currPolyIndexes.size() - 2; j++){
                    newPolyIndexes.push_back(currPolyIndexes.at(0));
                    newPolyIndexes.push_back(currPolyIndexes.at(i));
                    newPolyIndexes.push_back(currPolyIndexes.at(i+1));
                    ++i;
                }
                newAllPolyIndexes->push_back(newPolyIndexes);
            }
            else{
                newAllPolyIndexes->push_back(currPolyIndexes);
            }
        }

        delete allPolyIndexes;
        allPolyIndexes = newAllPolyIndexes;
    }
    void clean(){
        poses.clear();
        norms.clear();
        texes.clear();
        indexes.clear();
        allPolyIndexes->clear();
        lastPolyIds.clear();
    }
};