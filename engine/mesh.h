#pragma once

#include <common.h>

class Mesh : public AMesh<Vertex>{
public:
    using AMesh::AMesh;
};

class MeshList : public AListO<Mesh>{

};
