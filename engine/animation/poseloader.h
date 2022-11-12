#pragma once

#include <animation/pose.h>
#include <common/loader.h>
#include <filesystem>

class PoseLoader : private ULoader {
public:
    PoseList* parseKeyPoses(const filesystem::path& filePath) {
        PoseList* keyPoses = new PoseList;

        ifstream f;
        f.open(filePath);

        if (f.fail()) {
            throw string("Cannot open file: ") + filePath.string();
        }

        string line;
        int switchCase;

        string token;
        size_t pAmount = -1;

        while (!f.eof()) {
            switchCase = -1;

            getline(f, line);
            removeExcessSpaces(line);

            token = bite(" ", line);

            if (token == "p") {
                switchCase = 0;
            }
            if (token == "l") {
                switchCase = 1;
            }
            if (token == "r") {
                switchCase = 2;
            }
            if (token == "s") {
                switchCase = 3;
            }
            if (token == "t") {
                switchCase = 4;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0:
                ++pAmount;
                keyPoses->push(*(new Pose()));

                bool end;
                while (true) {
                    keyPoses->at(pAmount)->ids.push_back(
                        biteInt(" ", line, end));
                    if (end)
                        break;
                }
                break;
            case 1:
                keyPoses->at(pAmount)->locations.push_back(parseVec3(line));
                break;
            case 2:
                keyPoses->at(pAmount)->rotations.push_back(parseVec3(line));
                break;
            case 3:
                keyPoses->at(pAmount)->scales.push_back(parseVec3(line));
                break;
            case 4:
                // this is an example of how this should work, need to add more
                // dependences
                if (bite(" ", line) == "default") {
                    // keyPoses->at(pAmount).timingFunction = [](float a) ->
                    // float {return a;};
                }
                if (bite(" ", line) == "exp") {
                    keyPoses->at(pAmount)->timingFunction =
                        [](float a) -> float { return glm::exp(-a * 5); };
                }

                float poseTime = biteFloat(" ", line);
                keyPoses->at(pAmount)->poseTime = poseTime;
                keyPoses->animationTime += poseTime;
                break;
            }
        }

        return keyPoses;
    }
};