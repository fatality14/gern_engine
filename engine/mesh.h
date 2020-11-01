#pragma once

#include <engine/common.h>

class Mesh{
public:
    //add some methods to change polygons
    vector<Vertex>* vertices;
    vector<uint>* indices;
    vector<uint> partEndVertexIds;
    vector<uint> partEndMtlIds;

    uint nVertices = 0;
    uint nIndices = 0;

    string name;

    Mesh(string name = "noname"){
        this->name = name;

        vertices = new vector<Vertex>();
        this->indices = new vector<uint>();
    }
    Mesh(const Vertex* vertices, uint nVertices, uint* indices, uint nIndices, string name = "noname"){
        this->vertices = new vector<Vertex>(vertices, vertices + nVertices);
        this->indices = new vector<uint>(indices, indices + nIndices);

        this->nVertices = nVertices;
        this->nIndices = nIndices;

        this->name = name;
    }
    ~Mesh(){
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

class MeshList : public List<Mesh>{

};
