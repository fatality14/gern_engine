#pragma once

#include <animation/joint.h>
#include <common/alist.h>
#include <common/common.h>
#include <mesh/amesh.h>
#include <mesh/skeletonvertex.h>

class SkeletonMesh : public AMesh<SkeletonVertex> {
public:
    using AMesh::AMesh;

    // add some methods to change polygons and joints

    Joint rootJoint;
    JointList joints;

    //TODO remove?
    void genJointsFromRoot() { joints.appendFromRootJoint(rootJoint); }
};

class SkeletonMeshList : public AListO<SkeletonMesh> {};
