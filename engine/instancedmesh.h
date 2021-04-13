#pragma once

#include <engine/common.h>
#include <engine/mesh.h>

class InstancedMesh{
public:
    //add some methods to change polygons
    vector<Vertex>* vertices;
    vector<uint>* indices;
    vector<uint> partEndVertexIds;
    vector<uint> partEndMtlIds;

    uint nVertices = 0;
    uint nIndices = 0;

    string name;

    InstancedMesh(Mesh* prototype, vector<glm::mat4> modelMatrices){
        this->name = prototype->name;

        vertices = new vector<Vertex>();

        *vertices = *prototype->vertices;

        this->indices = new vector<uint>();
    }
    InstancedMesh(const Vertex* vertices, uint nVertices, uint* indices, uint nIndices, string name = "noname"){
        this->vertices = new vector<Vertex>(vertices, vertices + nVertices);
        this->indices = new vector<uint>(indices, indices + nIndices);

        this->nVertices = nVertices;
        this->nIndices = nIndices;

        this->name = name;
    }
    ~InstancedMesh(){
        delete vertices;
        delete indices;
    }

    void pushVertex(Vertex v){
        vertices->push_back(v);
    }
    void addPolyByIndices(uint a, uint b, uint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};

class InstancedMeshList : public List<InstancedMesh>{

};
