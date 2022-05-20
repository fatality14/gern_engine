#pragma once

#include <common/common.h>
#include <common/loader.h>
#include <common/list.h>
#include <animation/joint.h>
#include <chrono>

class AnimationPose : public ICommon{
public:
    vector<glm::vec3> locations;
    vector<glm::vec3> rotations;
    vector<glm::vec3> scales;
    vector<uint> ids;
    float poseTime;
    float(*timingFunction)(float) = [](float a) -> float {return a;};

    void appendPosition(glm::vec3 location, glm::vec3 rotation, glm::vec3 scale, uint id){
        locations.push_back(location);
        rotations.push_back(rotation);
        scales.push_back(scale);
        ids.push_back(id);
    }
    size_t getPositionsAmount(){
        return ids.size();
    }
};


class Animation : private ULoader{
public:
    float animationTimeMult;
    float currAnimationTime;

    string name;

    Animation(JointList& jointsToAnimate, float animationTimeMult = 1, float startTime = 0, string name = "noname"){
        joints = &jointsToAnimate;
        this->startTime = startTime;
        reset();
        this->animationTimeMult = animationTimeMult;
        currAnimationTime = 0;
        this->name = name;
    }
    void applyCurrPose(){
        calcCurrPose();
        applyPose(currPose);
    }
    void parseKeyPoses(string filePath){
        keyPoses.clear();

        ifstream f;
        f.open(filePath);

        if(f.fail()){
            throw string("Cannot open file: ") + filePath;
        }

        string line;
        int switchCase;
        bool end;

        string token;
        size_t pAmount = -1;

        while(!f.eof()){
            AnimationPose newPose;

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
            if(token == "t"){
                switchCase = 4;
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
            case 4:
                //this is an example of how this should work, need to add more dependences
                if(bite(" ", line, end) == "default"){
                    //keyPoses.at(pAmount).timingFunction = [](float a) -> float {return a;};
                }
                if(bite(" ", line, end) == "exp"){
                    keyPoses.at(pAmount).timingFunction = [](float a) -> float {return glm::exp(-a*5);};
                }

                float poseTime = stof(bite(" ", line, end));
                keyPoses.at(pAmount).poseTime = poseTime;
                animationTime += poseTime;
                break;
            }

        }
        if(startTime > animationTime){
            throw string("animation start time is higher than animation time");
        }
    }
    void addPose(AnimationPose& p){
        keyPoses.push_back(p);
        animationTime += p.poseTime;
    }
    void setStartTime(float time){
        startTime = time;
    }
    void reset(){
        auto additionalTime = std::chrono::milliseconds((long long)startTime * 1000);
        startTimeTimer = std::chrono::steady_clock::now() - additionalTime;

        timePassed = 0;
        currPoseIndex = 0;
    }
    float getAnimationTime(){
        return animationTime;
    }
    float getCurrTime(){
        return currAnimationTime;
    }

private:
    float animationTime = 0;
    float startTime = 0;

//    std::chrono::duration<long long, std::ratio<1, 1000000000>> snapTime;

    std::chrono::steady_clock::time_point startTimeTimer;
    std::chrono::steady_clock::time_point currTimeTimer;
    vector<AnimationPose> keyPoses;
    AnimationPose currPose;
    JointList* joints;
    float keyPoseTime;
    float interpolationCoef;

    float timePassed = 0;
    size_t currPoseIndex = 0;

    void calcCurrPose(){
        currTimeTimer = std::chrono::steady_clock::now();
        currAnimationTime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(currTimeTimer - startTimeTimer).count()/1000;

        keyPoseTime = keyPoses.at(currPoseIndex).poseTime * animationTimeMult;
        interpolationCoef = (currAnimationTime-timePassed)/keyPoseTime;
        interpolationCoef = keyPoses[currPoseIndex].timingFunction(interpolationCoef);
//        cout << currAnimationTime << " " << interpolationCoef << " " << currPoseIndex << endl;
        if(interpolationCoef > 1){
            while(interpolationCoef > 1){
                ++currPoseIndex;
                timePassed += keyPoseTime;
                --interpolationCoef;
            }
        }
        else if(currPoseIndex + 1 < keyPoses.size()){
            currPose = interpolatePoses(keyPoses[currPoseIndex], keyPoses[currPoseIndex+1], interpolationCoef);
        }
        else{
            startTimeTimer = std::chrono::steady_clock::now();

            timePassed = 0;
            currPoseIndex = 0;
        }
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
};

class AnimationList : public AListO<Animation>{};
