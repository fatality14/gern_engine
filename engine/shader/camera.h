#pragma once

#include <common/common.h>
#include <common/list.h>

class Camera : public ICommon{
public:
    string name;

    glm::vec3 location;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;

    GLfloat pitch = 0.f;
    GLfloat yaw = -90.f;
    GLfloat roll = 0.f;

    GLfloat movementSpeed = 0.1f;
    GLfloat sensitivity = 5.f;

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
