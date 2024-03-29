#pragma once

#include <buffer/meshbuffer.h>
#include <object/aobject.h>
#include <texture/skyboxtexture.h>
#include <texture/texture.h>

class SkyboxObject : public AObject<MeshBuffer> {
public:
    SkyboxTexture* skyboxTexture;

    SkyboxObject(Window& w, SkyboxTexture& st, MeshBuffer& b, Perspective& p,
                 View& v, const string& name = "noname")
        : AObject(w, b, p, v, name) {
        skyboxTexture = &st;

        shaderFields.push(*view);
        shaderFields.push(*skyboxTexture);
        shaderFields.push(*position);
        shaderFields.push(*perspective);
    }
    ~SkyboxObject() { delete skyboxTexture; }

    inline static GLuint currShaderId = -1;

    virtual void draw([[maybe_unused]] int flags = 0) override {
        window->setDrawOrder(false);
        shader->bind();

        if (currShaderId != shader->program) {
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);

            currShaderId = shader->program;
        }

        shaderFields.pushToShader(*shader);

        position->setDefaultEvents(*window); // remove this later

        buffer->bind();

        GLDB(glDrawArrays(GL_TRIANGLES, 0, buffer->getMesh().nVertices));

        buffer->unbind();
        shader->unbind();
        window->setDrawOrder(true);
    }

    void setTexture(SkyboxTexture& st) { skyboxTexture = &st; }
    SkyboxTexture& getTexture() { return *skyboxTexture; }
};

class SkyboxList : public AListO<SkyboxObject> {};
