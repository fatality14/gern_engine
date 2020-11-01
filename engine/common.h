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
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
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
private:
    vector<T*> list;
};
