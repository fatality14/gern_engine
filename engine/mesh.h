#pragma once

#include <engine/common.h>

class Mesh{
public:
    //add some methods to change polygons
    vector<Vertex>* vertices;
    vector<GLuint>* indices;
    vector<int> partEndVertexIds;

    unsigned int nVertices = 0;
    unsigned int nIndices = 0;

    string name;

    Mesh(string name = "noname"){
        this->name = name;

        vertices = new vector<Vertex>();
        this->indices = new vector<GLuint>();
    }
    Mesh(const Vertex* vertices, unsigned int nVertices, GLuint* indices, unsigned int nIndices, string name = "noname"){
        this->vertices = new vector<Vertex>(vertices, vertices + nVertices);
        this->indices = new vector<GLuint>(indices, indices + nIndices);

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
    void addPolyByIndices(GLuint a, GLuint b, GLuint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};

class MeshList : public List<Mesh>{

};
