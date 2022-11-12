#pragma once

#include <mesh/imesh.h>
#include <mesh/ivertex.h>

template <class T> class AMesh : public IMesh {
public:
    //TODO add some methods to change polygons
    vector<T>* vertices;
    vector<GLint> partEndVertexIds;
    vector<GLint> partEndMtlIds;

    GLint nVertices = 0;

    string name;

    AMesh(const string& name = "noname") {
        static_assert(std::is_base_of<IVertex, T>::value,
                      "Template parameter must be derived from IVertex");

        this->name = name;

        vertices = new vector<T>();
    }
    //TODO refactor vertices arg
    AMesh(const T* vertices, GLint nVertices,
          const string& name = "noname") {
        static_assert(std::is_base_of<IVertex, T>::value,
                      "Template parameter must be derived from IVertex");

        this->vertices = new vector<T>(vertices, vertices + nVertices);

        this->nVertices = nVertices;

        this->name = name;
    }
    virtual ~AMesh() {
        delete vertices;
    }

    void pushVertex(T v) { vertices->push_back(v); }
};
