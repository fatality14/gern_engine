#pragma once

#include <common/common.h>
#include <mesh/ivertex.h>

struct Vertex : public IVertex{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};
