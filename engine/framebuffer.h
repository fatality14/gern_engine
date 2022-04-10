#pragma once

#include <common.h>
#include <texture.h>

class Framebuffer{
public:
    GLuint FBO;
    GLuint RBO;
    TextureList* textureColorBuffers;

    string name;

    int width;
    int height;

    Framebuffer(int width, int height, string name = "noname"){
        this->width = width;
        this->height = height;
        this->textureColorBuffers = new TextureList;
        textureColorBuffers->addLayouts(1);

        this->name = name;
        textureColorBuffers->name = name;

        glGenFramebuffers(1, &FBO);
        glGenRenderbuffers(1, &RBO);
    }
    ~Framebuffer(){
        delete textureColorBuffers;
        glDeleteBuffers(1, &FBO);
        glDeleteBuffers(1, &RBO);
    }

    void genTextureColorBuffers(size_t amount){
        for(size_t i = 0; i < amount; i++){
            Texture* t = new Texture("textureColorBuffer"+to_string(i));

            t->setTextureSize(width, height);

            textureColorBuffers->push(*t);
            textureColorBuffers->appendTextureToLayout(0, i, i, "textureColorBuffer"+to_string(i));

            genColorAttachmentFramebuffer(i);
        }
        attachRenderBuffer();
    }
    void bind(){
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }
    void unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void bindTextureColorBuffer(size_t attachmentNum){
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentNum);
    }
private:
    void genColorAttachmentFramebuffer(GLuint attachmentNum){
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        textureColorBuffers->at(attachmentNum)->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + attachmentNum,
                               GL_TEXTURE_2D,
                               textureColorBuffers->at(attachmentNum)->textureId,
                               0);
    }
    void attachRenderBuffer(){
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Framebuffer is not complete!" << endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

class FramebufferList : public AList<Framebuffer>{

};
