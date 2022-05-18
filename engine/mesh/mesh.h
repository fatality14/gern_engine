#pragma once

#include <common/common.h>
#include <common/list.h>
#include <mesh/imesh.h>
#include <mesh/vertex.h>

class Mesh : public AMesh<Vertex>{
public:
    using AMesh::AMesh;
};

class MeshList : public AListO<Mesh>{

};
