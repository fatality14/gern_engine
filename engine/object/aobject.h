#pragma once

#include "common/iposition.h"
#include <buffer/meshbuffer.h>
#include <object/iobject.h>
#include <render/window.h>
#include <shader/ishaderfield.h>
#include <shader/perspective.h>
#include <shader/position.h>
#include <shader/view.h>
#include <texture/texture.h>

template <class T> class AObject : public IObject, public IPosition {
public:
    Window* window;
    T* buffer;
    Shader* shader; // shader same as in buffer
    Position* position;
    View* view;
    Perspective* perspective;
    ShaderFieldList shaderFields;

    string name;

    GLuint drawmode = GL_TRIANGLES;

    AObject(Window& w, T& b, Perspective& p, View& v, string name = "noname") {
        static_assert(std::is_base_of<IBuffer, T>::value,
                      "Template parameter T must be derived from IBuffer");

        if (w.getWindowPtr() != p.__getWindowPtr() ||
            p.__getWindowPtr() != v.__getWindowPtr()) {
            cout << "Perspective and view passed in \"Object\" constructor "
                    "must have pointers to the same \"Window\" object\n";
            cout << "Object not created\n";
            return;
        }

        window = &w;
        buffer = &b;
        shader = &buffer->getShaderPtr();
        perspective = &p;
        view = &v;

        this->name = name;

        position = new Position();
    }
    virtual ~AObject() { delete position; }

    AObject(AObject& t) = delete;

    void setDrawMode(GLuint GL_drawmode) { drawmode = GL_drawmode; }

    virtual void draw(int flags = 0) = 0;

    const string& getDrawMeshName() { return buffer->getMeshName(); }
    void move(float x, float y, float z) override { position->move(x, y, z); }
    void move(glm::vec3 location) override { position->move(location); }
    void moveTo(float x, float y, float z) override { position->moveTo(x, y, z); }
    void moveTo(glm::vec3 location) override { position->moveTo(location); }
    void rotate(float x, float y, float z) override { position->rotate(x, y, z); }
    void rotate(glm::vec3 rotation) override { position->rotate(rotation); }
    void rotateTo(float x, float y, float z) override{ position->rotateTo(x, y, z); }
    void rotateTo(glm::vec3 rotation) override { position->rotateTo(rotation); }
    void scaleBy(float x, float y, float z) override { position->scaleBy(x, y, z); }
    void scaleBy(glm::vec3 scale) override { position->scaleBy(scale); }
    void scaleTo(float x, float y, float z) override { position->scaleTo(x, y, z); }
    void scaleTo(glm::vec3 scale) override { position->scaleTo(scale); }
};
