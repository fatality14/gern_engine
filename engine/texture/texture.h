#pragma once

#include <common/common.h>
#include <shader/shaderfield.h>
#include <texture/itexture.h>
#include <common/list.h>

class Texture : public ITexture, public IShaderField{
public:
    GLuint textureId;
    GLuint textureUnit;

    Texture(){
        GLDB(glGenTextures(1, &textureId));
    }
    Texture(Texture& t) = delete;
    ~Texture(){
        GLDB(glDeleteTextures(1, &textureId));
    }

    void setUniformName(string uniformName){
        this->name = uniformName;
    }
    void setShaderParams(GLuint texuteUnit, string uniformName){
        this->textureUnit = texuteUnit;
        this->name = uniformName;
    }
    void pushToShader(Shader& shader) override{
        shader.setUniform1i(name, textureUnit);

        GLDB(glActiveTexture(GL_TEXTURE0 + textureUnit));
        bind();
    }
    void bind(){
        GLDB(glBindTexture(GL_TEXTURE_2D, textureId));
    }

    void setLoadParams(string path){
        this->path = path;
    }
    void loadTexture() override{
        image_width = 0;
        image_height = 0;

        image = SOIL_load_image(path.data(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

        GLDB(glBindTexture(GL_TEXTURE_2D, textureId));

        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLDB(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));

        if(image){
            GLDB(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
            GLDB(glGenerateMipmap(GL_TEXTURE_2D));
        }
        else{
            throw string("Texture ") + path + " loading failed";
        }

        delete[] image;
        image = nullptr;
    }

    void setTextureSize(GLsizei width, GLsizei height){
        image_width = width;
        image_height = height;
    }
    GLsizei getTextureWidth(){
        return image_width;
    }
    GLsizei getTextureHeight(){
        return image_height;
    }

    void setNewTextureData(unsigned char* data){
        bind();
        GLDB(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
        GLDB(glGenerateMipmap(GL_TEXTURE_2D));
    }

    unsigned char& loadDataFromShader(){
        if(image != nullptr){
            delete[] image;
            image = nullptr;
        }

        image = new unsigned char[image_width * image_height * 4];
        bind();
        GLDB(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));

//        int save_result = SOIL_save_image
//            (
//                "new_terrain.bmp",
//                SOIL_SAVE_TYPE_BMP,
//                image_width, image_height, 4,
//                c
//            );
        return *image;
    }
private:
    GLsizei image_width;
    GLsizei image_height;

    string path;

    unsigned char* image = nullptr;

};

class TextureList : public AListO<Texture>, public IShaderField{
private:
    struct TextureLayout{
        void pushTexture(GLuint texutreUnit, size_t textureIndex, string uniformName){
            texutreUnits.push_back(texutreUnit);
            textureIndexes.push_back(textureIndex);
            uniformNames.push_back(uniformName);
            ++layoutSize;
        }

        size_t layoutSize = 0;
        vector<GLuint> texutreUnits;
        vector<size_t> textureIndexes;
        vector<string> uniformNames;
    };

    vector<TextureLayout> texLayouts;

    void pushTextureToShaderByIndex(Shader* shader, GLuint n, size_t textureIndex, string uniformName){
        at(textureIndex)->setShaderParams(n, uniformName);
        at(textureIndex)->pushToShader(*shader);
    }

    size_t layoutId = 0;
public:
    TextureList(string name = "noname"){
        this->name = name;
    }

    void loadNew(string path){
        Texture* t = new Texture;

        t->setLoadParams(path);
        t->loadTexture();
        push(*t);
        unbindTextures();
    }
    void unbindTextures(){
        //no need for GLDB here, cause unbind is undefined in GL specs
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void appendTextureToLayout(size_t layoutId, GLuint textureUnit, size_t textureIndex, string uniformName){
        texLayouts.at(layoutId).pushTexture(textureUnit, textureIndex, uniformName);
    }
    void addLayouts(size_t amount){
        for(size_t i = 0; i < amount; ++i){
            texLayouts.push_back(TextureLayout());
        }
    }

    void setShaderParams(size_t layoutId){
        this->layoutId = layoutId;
    }
    void pushToShader(Shader& shader) override{
        TextureLayout* currLayout = &texLayouts.at(layoutId);
        for(size_t i = 0; i < currLayout->textureIndexes.size(); ++i){            
            pushTextureToShaderByIndex(&shader,
                               currLayout->texutreUnits.at(i),
                               currLayout->textureIndexes.at(i),
                               currLayout->uniformNames.at(i));
        }
    }
    size_t layoutsAmount(){
        return texLayouts.size();
    }

    string name;
};
