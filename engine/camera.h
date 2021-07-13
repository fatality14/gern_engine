#pragma once

#include <common.h>

class Camera{
public:
    string name;

    glm::vec3 location;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;

    GLfloat pitch = 0.f;
    GLfloat yaw = -90.f;
    GLfloat roll = 0.f;

    GLfloat movementSpeed = 3.f;
    GLfloat sensitivity = 5.f;

    Camera(string name = "noname"){
        this->name = name;

        location = glm::vec3(0.f, 0.f, 1.f);
        up = glm::vec3(0.f, 1.f, 0.f);
        front = glm::vec3(0.f, 0.f, -1.f);
        right = glm::vec3(0.f);
    }
};

class CameraList : public List<Camera>{

};
