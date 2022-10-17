#pragma once

#include <common/common.h>
#include <mesh/ivertex.h>

struct SkeletonVertex : public IVertex {
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
    glm::vec3 joints;
    glm::vec3 weights;
};
