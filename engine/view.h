#pragma once

#include <common.h>
#include <window.h>
#include <mouselistener.h>
#include <camera.h>
#include <shader.h>

class View{
public:
    View(Window& w, MouseListener& ml, Camera& c){
        window = &w;
        mouse = &ml;
        camera = &c;

        worldUp = glm::vec3(0.f, 1.f, 0.f);

        viewMatrix = glm::lookAt(camera->location, camera->location + camera->front, camera->up);
    }

    void pushToShader(Shader* s, string viewMatrixUniformName, string cameraPosUniformName){
        updateMatrices();
        s->setUniformMatrix4fv(viewMatrixUniformName, glm::value_ptr(viewMatrix));
        s->setUniform3fv(cameraPosUniformName, glm::value_ptr(camera->location));
    }
    void updateMatrices(){
        updateCameraPosition();
        //camera->position + camera->front is camera direction vector
        viewMatrix = glm::lookAt(camera->location, camera->location + camera->front, camera->up);
    }
    void setDefaultEvents(){
        setEvent(window->getWindowPtr(), A, camera->location -= camera->right * camera->movementSpeed * mouse->dt);
        setEvent(window->getWindowPtr(), D, camera->location += camera->right * camera->movementSpeed * mouse->dt);
        setEvent(window->getWindowPtr(), Q, camera->location -= camera->up * camera->movementSpeed * mouse->dt);
        setEvent(window->getWindowPtr(), E, camera->location += camera->up * camera->movementSpeed * mouse->dt);
        setEvent(window->getWindowPtr(), S, camera->location -= camera->front * camera->movementSpeed * mouse->dt);
        setEvent(window->getWindowPtr(), W, camera->location += camera->front * camera->movementSpeed * mouse->dt)
    }
    void setCamera(Camera& c){
        camera = &c;
    }
    Camera& getCamera(){
        return *camera;
    }
    const GLFWwindow* getWindowPtr(){
        return  window->getWindowPtr();
    }
    glm::mat4& getViewMatrix(){
        return viewMatrix;
    }
private:
    Window* window;
    MouseListener* mouse;

    Camera* camera;

    glm::mat4 viewMatrix;
    glm::vec3 worldUp;

    void updateCameraPosition(){
        camera->pitch += static_cast<GLfloat>(mouse->mouseOffsetY) * camera->sensitivity * mouse->dt;
        camera->yaw += static_cast<GLfloat>(mouse->mouseOffsetX) * camera->sensitivity * mouse->dt;

        if(camera->pitch > 80.f)
            camera->pitch = 80.f;
        else if (camera->pitch < -80.f)
            camera->pitch = -80.f;

        if(camera->yaw > 360.f || camera->yaw < -360.f){
            camera->yaw = 0;
        }

        //hypotenuse = 1, so yaw and pitch projection calculates by cosA = x/h = x, sinA = y
        //the coefficient is relative cause cos * cos and other projection combinations gives number in range [-1;1]
        camera->front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        camera->front.y = sin(glm::radians(camera->pitch));
        camera->front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));

        camera->front = glm::normalize(camera->front);
        camera->right = glm::normalize(glm::cross(camera->front, worldUp));
        camera->up = glm::normalize(glm::cross(camera->right, camera->front));
    }
};
