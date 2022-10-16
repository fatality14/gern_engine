#pragma once

#include <common/common.h>
#include <common/alist.h>
#include <mesh/amesh.h>
#include <mesh/skeletonvertex.h>
#include <animation/joint.h>

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

class SkeletonMeshList : public AListO<SkeletonMesh>{

};
