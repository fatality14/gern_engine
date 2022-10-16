#pragma once

#include <common/common.h>
#include <common/alist.h>
#include <mesh/amesh.h>
#include <mesh/vertex.h>

class Mesh : public AMesh<Vertex>{
public:
    using AMesh::AMesh;
};

class MeshList : public AListO<Mesh>{

};
