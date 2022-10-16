#pragma once

#include <common/common.h>
#include <common/alist.h>

class Pose : public ICommon{
public:
    vector<glm::vec3> locations;
    vector<glm::vec3> rotations;
    vector<glm::vec3> scales;
    vector<size_t> ids;
    float poseTime;
    float(*timingFunction)(float) = [](float a) -> float {return a;};

    void appendPosition(glm::vec3 location, glm::vec3 rotation, glm::vec3 scale, size_t id){
        locations.push_back(location);
        rotations.push_back(rotation);
        scales.push_back(scale);
        ids.push_back(id);
    }
    size_t getPositionsAmount(){
        return ids.size();
    }
};

class PoseList : public AListO<Pose>{
    public:
    float animationTime = 0;

    void addPose(Pose &p) {
        push(p);
        animationTime += p.poseTime;
    }
};