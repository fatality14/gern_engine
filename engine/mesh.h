#pragma once

#include <common.h>

class Mesh : public AMesh<Vertex>{
public:
    using AMesh::AMesh;

    //add some methods to change polygons
};

class MeshList : public AList<Mesh>{

};
