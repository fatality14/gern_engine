#pragma once

#include <skeletonmesh.h>
#include <abuffer.h>

//maybe add BufferList
class SkeletonBuffer : public ABuffer<SkeletonMesh, SkeletonVertex, SkeletonBuffer>{
public:
    using ABuffer::ABuffer;

    static void specifyAttribPointers(Shader* s){
        s->setFloatAttribPointer<SkeletonVertex>("vertex_position", 3, offsetof(SkeletonVertex, position));
        s->setFloatAttribPointer<SkeletonVertex>("vertex_texcoord", 2, offsetof(SkeletonVertex, texcoord));
        s->setFloatAttribPointer<SkeletonVertex>("vertex_normal", 3, offsetof(SkeletonVertex, normal));
        s->setFloatAttribPointer<SkeletonVertex>("vertex_joints", 3, offsetof(SkeletonVertex, joints));
        s->setFloatAttribPointer<SkeletonVertex>("vertex_weights", 3, offsetof(SkeletonVertex, weights));
    }
};
