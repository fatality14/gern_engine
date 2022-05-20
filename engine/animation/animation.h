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
    float timer;
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

    string name = "noname";

    Animation(JointList& jointsToAnimate, float animationTimeMult = 0){
        joints = &jointsToAnimate;
        startTime = std::chrono::steady_clock::now();
        this->animationTimeMult = animationTimeMult;
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

        if(f.fail()){
            throw string("Cannot open file: ") + filePath;
        }

        string line;
        int switchCase;
        bool end;
        size_t pAmount = -1;

        string token;

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

                float timer = stof(bite(" ", line, end));
                keyPoses.at(pAmount).timer = timer;
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

    float timePassed = 0;
    size_t currPoseIndex = 0;

    void calcCurrPose(){
        currTime = std::chrono::steady_clock::now();
        currAnimationTime = (float)std::chrono::duration_cast<std::chrono::milliseconds>(currTime - startTime).count()/1000;

        keyPoseTime = keyPoses.at(currPoseIndex).timer * animationTimeMult;
        interpolationCoef = (currAnimationTime-timePassed)/keyPoseTime;
        interpolationCoef = keyPoses[currPoseIndex].timingFunction(interpolationCoef);
//        cout << currAnimationTime << " " << interpolationCoef << " " << currPoseIndex << endl;
        if(interpolationCoef > 1){
            ++currPoseIndex;
            timePassed += keyPoseTime;
        }
        else if(currPoseIndex + 1 < keyPoses.size()){
            currPose = interpolatePoses(keyPoses[currPoseIndex], keyPoses[currPoseIndex+1], interpolationCoef);
        }
        else{
            currPoseIndex = 0;

            startTime = std::chrono::steady_clock::now();
            timePassed = 0;
            currAnimationTime = 0;
        }

//        keyPoseTime = animationTimeMult/(keyPoses.size()-1);
//        uint keyPoseIndex = currAnimationTime/keyPoseTime;
//        interpolationCoef = currAnimationTime/keyPoseTime - keyPoseIndex;
//        //keyPoses[keyPoseIndex].timingFunction = [](float a) -> float {return glm::exp(-a*9);};
//        interpolationCoef = keyPoses[keyPoseIndex].timingFunction(interpolationCoef);
//        //cout << currAnimationTime << " " << interpolationCoef << " " << keyPoseIndex << endl;
//        if(keyPoseIndex + 1 < keyPoses.size())
//            currPose = interpolatePoses(keyPoses[keyPoseIndex], keyPoses[keyPoseIndex+1], interpolationCoef);
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
