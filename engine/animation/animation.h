#pragma once

#include <animation/joint.h>
#include <animation/pose.h>
#include <animation/poseloader.h>
#include <chrono>
#include <common/alist.h>
#include <common/common.h>
#include <common/loader.h>

class Animation : private ULoader {
public:
    float animationTimeMult;
    float currAnimationTime;

    string name;
    JointList* joints;
    PoseList keyPoses;

    Animation(JointList& jointsToAnimate, float animationTimeMult = 1,
              float startTime = 0, string name = "noname") {
        joints = &jointsToAnimate;
        this->startTime = startTime;
        reset();

        this->animationTimeMult = animationTimeMult;
        currAnimationTime = 0;
        this->name = name;
    }
    void applyCurrPose() {
        calcCurrPose();
        applyPose(currPose);
    }
    void setKeyPoses(const PoseList& keyPoses) { this->keyPoses = keyPoses; }
    void setStartTime(float time) { startTime = time; }
    void reset() {
        auto additionalTime =
            std::chrono::milliseconds((long long)startTime * 1000);
        startTimeTimer = std::chrono::steady_clock::now() - additionalTime;

        timePassed = 0;
        currPoseIndex = 0;
    }
    float getCurrTime() { return currAnimationTime; }

private:
    float startTime = 0;

    //    std::chrono::duration<long long, std::ratio<1, 1000000000>> snapTime;

    std::chrono::steady_clock::time_point startTimeTimer;
    std::chrono::steady_clock::time_point currTimeTimer;
    Pose currPose;
    float keyPoseTime;
    float interpolationCoef;

    float timePassed = 0;
    size_t currPoseIndex = 0;

    void calcCurrPose() {
        currTimeTimer = std::chrono::steady_clock::now();
        currAnimationTime =
            (float)std::chrono::duration_cast<std::chrono::milliseconds>(
                currTimeTimer - startTimeTimer)
                .count() /
            1000;

        if (currPoseIndex < keyPoses.size()) {
            keyPoseTime =
                keyPoses.at(currPoseIndex)->poseTime * animationTimeMult;
            interpolationCoef = (currAnimationTime - timePassed) / keyPoseTime;
            interpolationCoef =
                keyPoses.at(currPoseIndex)->timingFunction(interpolationCoef);
            //        cout << currAnimationTime << " " << interpolationCoef << "
            //        " << currPoseIndex << endl;
            if (interpolationCoef > 1) {
                while (interpolationCoef > 1) {
                    ++currPoseIndex;
                    timePassed += keyPoseTime;
                    --interpolationCoef;
                }
            } else if (currPoseIndex + 1 < keyPoses.size()) {
                currPose = interpolatePoses(*keyPoses.at(currPoseIndex),
                                            *keyPoses.at(currPoseIndex + 1),
                                            interpolationCoef);
            } else {
                startTimeTimer = std::chrono::steady_clock::now();

                timePassed = 0;
                currPoseIndex = 0;
            }
        } else {
            reset();
        }
    }
    void applyPose(Pose& p) {
        for (size_t i = 0; i < p.getPositionsAmount(); ++i) {
            joints->getById(p.ids.at(i)).moveTo(p.locations.at(i));
            joints->getById(p.ids.at(i)).rotateTo(p.rotations.at(i));
            joints->getById(p.ids.at(i)).scaleTo(p.scales.at(i));
        }
    }
    Pose interpolatePoses(Pose& p1, Pose& p2, float interpolationCoef) {
        Pose p;
        for (size_t i = 0; i < p1.getPositionsAmount(); ++i) {
            p.locations.push_back(p1.locations.at(i) * (1 - interpolationCoef) +
                                  p2.locations.at(i) * interpolationCoef);
            p.rotations.push_back(p1.rotations.at(i) * (1 - interpolationCoef) +
                                  p2.rotations.at(i) * interpolationCoef);
            p.scales.push_back(p1.scales.at(i) * (1 - interpolationCoef) +
                               p2.scales.at(i) * interpolationCoef);
            p.ids.push_back(p1.ids.at(i));
        }
        return p;
    }
};

class AnimationList : public AListO<Animation> {};
