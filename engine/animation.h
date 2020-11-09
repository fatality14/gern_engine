#pragma once

#include <engine/common.h>
#include <engine/joint.h>
#include <chrono>

class AnimationPose{
public:
    vector<glm::vec3> locations;
    vector<glm::vec3> rotations;
    vector<glm::vec3> scales;
    vector<uint> ids;

    size_t getPositionsAmount(){
        return ids.size();
    }
};


class Animation : private Loader{
public:
    float animationTime;
    float currAnimationTime;

    Animation(JointList& jointsToAnimate, float animationTime = 0){
        joints = &jointsToAnimate;
        startTime = std::chrono::steady_clock::now();
        this->animationTime = animationTime;
        currAnimationTime = 0;
    }
    void applyCurrPose(){
        calcCurrPose();
        applyPose(currPose);
    }
    void parseKeyPoses(string filePath){
        keyPoses.clear();

        ifstream f;
        f.open(filePath);

        string line;
        int switchCase;
        bool end;
        size_t pAmount = -1;

        while(!f.eof()){
            switchCase = -1;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "p"){
                switchCase = 0;
            }
            if(token == "l"){
                switchCase = 1;
            }
            if(token == "r"){
                switchCase = 2;
            }
            if(token == "s"){
                switchCase = 3;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0:
                ++pAmount;
                keyPoses.push_back(AnimationPose());

                bool end;
                while (true){
                    token = bite(" ", line, end);
                    keyPoses.at(pAmount).ids.push_back(stoi(token));
                    if(end)
                        break;
                }
                break;
            case 1:
                keyPoses.at(pAmount).locations.push_back(parseVec3(line));
                break;
            case 2:
                keyPoses.at(pAmount).rotations.push_back(parseVec3(line));
                break;
            case 3:
                keyPoses.at(pAmount).scales.push_back(parseVec3(line));
                break;
            }
        }
    }
    void addPose(AnimationPose& p){
        keyPoses.push_back(p);
    }

private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currTime;
    vector<AnimationPose> keyPoses;
    AnimationPose currPose;
    JointList* joints;
    float keyPoseTime;
    float interpolationCoef;

    void calcCurrPose(){
        calcCurrAnimationTime();
        keyPoseTime = animationTime/(keyPoses.size()-1);
        uint keyPoseIndex = currAnimationTime/keyPoseTime;
        interpolationCoef = currAnimationTime/keyPoseTime - keyPoseIndex;
//        cout << currAnimationTime << " " << interpolationCoef << " " << keyPoseIndex << endl;
        if(keyPoseIndex + 1 < keyPoses.size())
            currPose = interpolatePoses(keyPoses[keyPoseIndex], keyPoses[keyPoseIndex+1], interpolationCoef);
    }
    void applyPose(AnimationPose& p){
        for(size_t i = 0; i < p.getPositionsAmount(); ++i){
            joints->getById(p.ids.at(i)).moveTo(p.locations.at(i));
            joints->getById(p.ids.at(i)).rotateTo(p.rotations.at(i));
            joints->getById(p.ids.at(i)).scaleTo(p.scales.at(i));
        }
    }
    AnimationPose interpolatePoses(AnimationPose& p1, AnimationPose& p2, float interpolationCoef){
        AnimationPose p;
        for(size_t i = 0; i < p1.getPositionsAmount(); ++i){
            p.locations.push_back(p1.locations.at(i)*(1-interpolationCoef) + p2.locations.at(i)*interpolationCoef);
            p.rotations.push_back(p1.rotations.at(i)*(1-interpolationCoef) + p2.rotations.at(i)*interpolationCoef);
            p.scales.push_back(p1.scales.at(i)*(1-interpolationCoef) + p2.scales.at(i)*interpolationCoef);
            p.ids.push_back(p1.ids.at(i));
        }
        return p;
    }
    void calcCurrAnimationTime(){
        currTime = std::chrono::steady_clock::now();
        currAnimationTime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime).count()/1000;
        if(currAnimationTime > animationTime){
            startTime = std::chrono::steady_clock::now();
            currAnimationTime = 0;
        }
    }
};
