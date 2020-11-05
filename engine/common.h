#pragma once

#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include <vector>

#define GLEW_STATIC

using namespace std;

typedef unsigned int uint;

#define setEvent(GLuint_window, Button, Code)\
    if(glfwGetKey(GLuint_window, GLFW_KEY_##Button) == GLFW_PRESS){\
        Code;\
    }\

float someCounter = 0;

//TODO: divide some functions to smaller pieces
//TODO: add notes that camera and mesh classes are just data containers, rename classes or smth else
//TODO: add shaderlist to renderer, let renderer choose which shader should be applied to object
//      user should be able to pass objects and skyboxes to shader from drawFunction
//      might make variable to check current active shader to check if its uniforms are already up-to-date
//      or make class ShaderUpdater to dinamically push data to shader
//TODO: make methods to access the objects of built-in classes
struct Vertex{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
};
struct SkeletonVertex{
    glm::vec3 position;
    glm::vec2 texcoord;
    glm::vec3 normal;
    glm::ivec3 joints;
    glm::vec3 weights;
};

struct uintvec3{
    uintvec3(uint x, uint y, uint z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    uint x;
    uint y;
    uint z;
};

template<class T>
class List{
public:
    //add check for repeating names in list

    ~List(){
        for(size_t i = 0; i < list.size(); i++){
            delete list.at(i);
        }
    }

    size_t size(){
        return list.size();
    }
    void push(T& obj){
        list.push_back(&obj);
    }
    void popByName(string name){
        for(size_t i = 0; i < list.size(); i++){
            if(list.at(i)->name == name){
                list.erase(list.begin() + i);
            }
        }
    }
    void popByIndex(size_t index){
        list.erase(list.begin() + index);
    }
    T* getByName(string name){
        for(size_t i = 0; i < list.size(); i++){
            if(list.at(i)->name == name){
                return list.at(i);
            }
        }
        return nullptr;
    }
    T* at(unsigned int i){
        return list.at(i);
    }
    void wipe(){
        for(size_t i = 0; i < list.size(); i++){
            delete list.at(i);
        }
        list.clear();
    }
private:
    vector<T*> list;
};

class Loader{
public:
    string token;

    int calcNumArgsDividedBy(string delimiter, string line){
        int numArgs = 0;
        string token;
        bool end;

        while(true){
            token = bite(delimiter, line, end);
            ++numArgs;
            if(end)
                break;
        }
        return numArgs;
    }

    void removeBadSpaces(string& line){
        size_t i = -1;
        while(true){
            ++i;
            if(i >= line.size() - 1 || line.size() < 2)
                break;
            if(line.at(i) == line.at(i+1) && line.at(i) == ' '){
                line.erase(line.begin()+i);
                --i;
            }
        }
        if(line.size() == 0)
            return;

        while(true){
            if(line.at(line.size()-1) == ' ')
                line.erase(line.begin() + line.size()-1);
            else
                break;
        }
        while(true){
            if(line.at(0) == ' ')
                line.erase(line.begin());
            else
                break;
        }
    }
    string& bite(const string& delimiter, string& line, bool& end){
        size_t pos = 0;
        string* token = new string;

        pos = line.find(delimiter);
        end = false;

        if(pos != string::npos){
            *token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
        }
        else{
            end = true;
            token = &line;
        }
        return *token;
    }
    glm::vec3& parseVec3(string& line){
        float norm[3];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 3)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec3(norm[0], norm[1], norm[2]);
    }
    glm::vec2& parseVec2(string& line){
        float norm[2];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 2)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec2(norm[0], norm[1]);
    }
};
