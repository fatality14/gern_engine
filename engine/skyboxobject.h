#pragma once

#include <aobject.h>
#include <buffer.h>
#include <skyboxtexture.h>
#include <texture.h>

class SkyboxObject : public AObject<Buffer>{
public:
    SkyboxTexture* skyboxTexture;

    TextureList tl;//remove this later

    SkyboxObject(Window& w, SkyboxTexture& st,
                 Buffer& b, Perspective& p,
                 View& v, string name = "noname")
        : AObject(w, tl, b, p, v, name)
    {
        skyboxTexture = &st;
    }
    ~SkyboxObject(){
        delete skyboxTexture;
    }

    virtual void draw(int flags = 0) override{
        window->setDrawOrder(false);
        shader->bind();

        position->pushToShader(shader, "modelMatrix");
        perspective->pushToShader(shader, "projectionMatrix");
        view->pushToShader(shader, "viewMatrix", "cameraPos");
        skyboxTexture->pushToShader(shader, 0, "skybox");

        position->setDefaultEvents(window);//remove this later

        buffer->bind();

        if (buffer->getMesh().nIndices != 0)
            glDrawElements(GL_TRIANGLES, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else
            glDrawArrays(GL_TRIANGLES, 0, buffer->getMesh().nVertices);

        buffer->unbind();
        shader->unbind();
        window->setDrawOrder(true);
    }

    void setTexture(SkyboxTexture& st){
        skyboxTexture = &st;
    }
    SkyboxTexture& getTexture(){
        return *skyboxTexture;
    }
};

class SkyboxList : public AList<SkyboxObject>{

};
