#pragma once

#include <engine/common.h>
#include <engine/mesh.h>

class SkeletonMesh{
public:
    //add some methods to change polygons
    vector<SkeletonVertex>* vertices;
    vector<uint>* indices;
    vector<uint> partEndVertexIds;
    vector<uint> partEndMtlIds;

    uint nVertices = 0;
    uint nIndices = 0;

    string name;

    SkeletonMesh(string name = "noname"){
        this->name = name;

        vertices = new vector<SkeletonVertex>();
        this->indices = new vector<uint>();
    }
    SkeletonMesh(const SkeletonVertex* vertices, uint nVertices, uint* indices, uint nIndices, string name = "noname"){
        this->vertices = new vector<SkeletonVertex>(vertices, vertices + nVertices);
        this->indices = new vector<uint>(indices, indices + nIndices);

        this->nVertices = nVertices;
        this->nIndices = nIndices;

        this->name = name;
    }
    ~SkeletonMesh(){
        delete vertices;
        delete indices;
    }

    void pushVertex(SkeletonVertex v){
        vertices->push_back(v);
    }
    void addPolyByIndices(uint a, uint b, uint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};

class SkeletonMeshList : public List<SkeletonMesh>{

};
