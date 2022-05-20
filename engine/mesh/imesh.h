#pragma once

#include <common/common.h>
#include <mesh/ivertex.h>

struct IMesh : public ICommon{};

//restrict T type with common vertex interface
template <class T>
class AMesh : public IMesh{
public:
    //add some methods to change polygons
    vector<T>* vertices;
    vector<GLint>* indices;
    vector<GLint> partEndVertexIds;
    vector<GLint> partEndMtlIds;

    GLint nVertices = 0;
    GLint nIndices = 0;

    string name;

    AMesh(string name = "noname"){
        static_assert(std::is_base_of<IVertex, T>::value, "Template parameter must be derived from IVertex");

        this->name = name;

        vertices = new vector<T>();
        this->indices = new vector<GLint>();
    }
    AMesh(const T* vertices, GLint nVertices, GLint* indices, GLint nIndices, string name = "noname"){
        static_assert(std::is_base_of<IVertex, T>::value, "Template parameter must be derived from IVertex");

        this->vertices = new vector<T>(vertices, vertices + nVertices);
        this->indices = new vector<GLint>(indices, indices + nIndices);

        this->nVertices = nVertices;
        this->nIndices = nIndices;

        this->name = name;
    }
    virtual ~AMesh(){
        delete vertices;
        delete indices;
    }

    void pushVertex(T v){
        vertices->push_back(v);
    }
    void addPolyByIndices(GLint a, GLint b, GLint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};
