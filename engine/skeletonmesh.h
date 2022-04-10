#pragma once

#include <common.h>
#include <joint.h>

class SkeletonMesh : public AMesh<SkeletonVertex>{
public:
    using AMesh::AMesh;

    //add some methods to change polygons and joints

    Joint rootJoint;
    JointList joints;

    //remove?
    void genJointsFromRoot(){
        joints.appendFromRootJoint(rootJoint);
    }
};

class SkeletonMeshList : public AList<SkeletonMesh>{

};
