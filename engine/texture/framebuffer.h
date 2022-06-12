#pragma once

#include <common/common.h>
#include <texture/texture.h>

class Framebuffer : public ICommon{
public:
    GLuint FBO;
    GLuint RBO;
    TextureList* textureColorBuffers;

    string name;

    GLsizei width;
    GLsizei height;

    Framebuffer(GLsizei width, GLsizei height, string name = "noname"){
        this->width = width;
        this->height = height;
        this->textureColorBuffers = new TextureList;
        textureColorBuffers->addLayouts(1);

        this->name = name;
        textureColorBuffers->name = name;

        GLDB(glGenFramebuffers(1, &FBO));
        GLDB(glGenRenderbuffers(1, &RBO));
    }
    ~Framebuffer(){
        delete textureColorBuffers;//a bug if someone takes TextureList control and clear it
        GLDB(glDeleteBuffers(1, &FBO));
        GLDB(glDeleteBuffers(1, &RBO));
    }

    void appendTextureColorBuffers(size_t amount){
        size_t tcbSize = textureColorBuffers->size();
        for(size_t i = tcbSize; i < tcbSize + amount; i++){
            Texture* t = new Texture;

            t->setUniformName("textureColorBuffer"+to_string(i));
            t->setTextureSize(width, height);

            textureColorBuffers->push(*t);
            textureColorBuffers->appendTextureToLayout(0, i, i, "textureColorBuffer"+to_string(i));

            genColorAttachmentFramebuffer(i);
        }
        attachRenderBuffer();
    }
    void bind(){
        GLDB(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    }
    void unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void bindTextureColorBuffer(GLuint attachmentNum){
        GLDB(glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentNum));
    }
private:
    void genColorAttachmentFramebuffer(GLuint attachmentNum){
        GLDB(glBindFramebuffer(GL_FRAMEBUFFER, FBO));

        textureColorBuffers->at(attachmentNum)->bind();
        GLDB(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));

        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GLDB(glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + attachmentNum,
                               GL_TEXTURE_2D,
                               textureColorBuffers->at(attachmentNum)->textureId,
                               0));
    }
    void attachRenderBuffer(){
        GLDB(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
        GLDB(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));

        GLDB(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO));

        GLenum checkComplete;
        GLDBR(checkComplete, glCheckFramebufferStatus(GL_FRAMEBUFFER));
        if (checkComplete!= GL_FRAMEBUFFER_COMPLETE)
            cout << "Framebuffer is not complete!" << endl;
        unbind();
    }
};

class FramebufferList : public AListO<Framebuffer>{

};
