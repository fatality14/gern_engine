#pragma once

#include <common/common.h>
#include <common/alist.h>

class Camera : public ICommon{
public:
    string name;

    glm::vec3 location;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;

    float pitch = 0.f;
    float yaw = -90.f;
    float roll = 0.f;

    float movementSpeed = 0.1f;
    float sensitivity = 5.f;

    Camera(glm::vec3 location, float movementSpeed = 0.1, float sensitivity = 5, string name = "noname"){
        this->name = name;

        this->movementSpeed = movementSpeed;
        this->sensitivity = sensitivity;

        this->location = location;
        up = glm::vec3(0.f, 1.f, 0.f);
        front = glm::vec3(0.f, 0.f, -1.f);
        right = glm::vec3(0.f);
    }
};

class CameraList : public AListO<Camera>{

};
