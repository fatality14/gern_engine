#pragma once

#include <common/common.h>

struct ILoader : public ICommon {};

// U stands for utility
class ULoader : public ILoader {
public:
    static size_t calcNumArgsDividedBy(string delimiter, string line) {
        size_t numArgs = 0;
        string token;
        bool end;

        while (true) {
            token = bite(delimiter, line, end);
            ++numArgs;
            if (end)
                break;
        }
        return numArgs;
    }

    static void removeBadSpaces(string& line) {
        size_t i = -1;
        while (true) {
            ++i;
            if (i >= line.size() - 1 || line.size() < 2)
                break;
            if (line.at(i) == line.at(i + 1) && line.at(i) == ' ') {
                line.erase(line.begin() + i);
                --i;
            }
        }
        if (line.size() == 0)
            return;

        while (true) {
            if (line.at(line.size() - 1) == ' ')
                line.erase(line.begin() + line.size() - 1);
            else
                break;
        }
        while (true) {
            if (line.at(0) == ' ')
                line.erase(line.begin());
            else
                break;
        }
    }
    static string bite(const string& delimiter, string& line, bool& end) {
        size_t pos = 0;
        string token;

        pos = line.find(delimiter);
        end = false;

        if (pos != string::npos) {
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
        } else {
            end = true;
            token = line;
        }
        return token;
    }
    static string bite(const string& delimiter, string& line) {
        size_t pos = 0;
        string token;

        pos = line.find(delimiter);

        if (pos != string::npos) {
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
        } else {
            token = line;
        }
        return token;
    }

    static float biteFloat(const string& delimiter, string& line, bool& end) {
        try {
            return stof(bite(delimiter, line, end));
        } catch (...) {
            throw string("wrong float format while parsing file");
        }
    }
    static float biteFloat(const string& delimiter, string& line) {
        try {
            return stof(bite(delimiter, line));
        } catch (...) {
            throw string("wrong float format while parsing file");
        }
    }

    static int biteInt(const string& delimiter, string& line, bool& end) {
        try {
            return stoi(bite(delimiter, line, end));
        } catch (...) {
            throw string("wrong int format while parsing file");
        }
    }
    static int biteInt(const string& delimiter, string& line) {
        try {
            return stoi(bite(delimiter, line));
        } catch (...) {
            throw string("wrong int format while parsing file");
        }
    }

    static glm::vec3 parseVec3(string& line) {
        float norm[3];
        int i = -1;
        bool end;

        while (true) {
            ++i;

            float v = biteFloat(" ", line, end);
            if (i < 3)
                norm[i] = v;
            if (end)
                break;
        }

        glm::vec3 ret = glm::vec3(norm[0], norm[1], norm[2]);
        return ret;
    }
    static glm::vec2 parseVec2(string& line) {
        float norm[2];
        int i = -1;
        bool end;

        while (true) {
            ++i;

            float v = biteFloat(" ", line, end);
            if (i < 2)
                norm[i] = v;
            if (end)
                break;
        }

        glm::vec2 ret = glm::vec2(norm[0], norm[1]);
        return ret;
    }
};
