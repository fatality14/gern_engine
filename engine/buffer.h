#pragma once

#include <mesh.h>
#include <abuffer.h>

//maybe add BufferList
class Buffer : public ABuffer<Mesh, Vertex, Buffer>{
public:
    using ABuffer::ABuffer;

    static void specifyAttribPointers(Shader* s){
        s->setFloatAttribPointer<Vertex>("vertex_position", 3, offsetof(Vertex, position));
        s->setFloatAttribPointer<Vertex>("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        s->setFloatAttribPointer<Vertex>("vertex_normal", 3, offsetof(Vertex, normal));
    }
};
