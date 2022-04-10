#pragma once

#include <common.h>
#include <shader.h>

class Texture{
public:
    GLuint textureId;
    string name;

    Texture(string name = "noname"){
        this->name = name;
        glGenTextures(1, &textureId);
    }
    Texture(Texture& t) = delete;
    ~Texture(){
        glDeleteTextures(1, &textureId);
    }
    void pushToShader(Shader* shader, GLint n, string uniformName){
        shader->setUniform1i(uniformName, n);

        glActiveTexture(GL_TEXTURE0 + n);
        bind();
    }
    void bind(){
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
    void loadTexture(string path){
        //load image
        image_width = 0;
        image_height = 0;

        image = SOIL_load_image(path.data(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

        //init and active texture
        glBindTexture(GL_TEXTURE_2D, textureId);

        //setup texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //set texture data and mipmap
        //mipmap is used to compress texture
        if(image){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            cout << "Texture loading failed" << endl;
        }

        //free space
        delete[] image;
        image = nullptr;
    }

    void setTextureSize(int width, int height){
        image_width = width;
        image_height = height;
    }
    int getTextureWidth(){
        return image_width;
    }
    int getTextureHeight(){
        return image_height;
    }

    void setNewTextureData(unsigned char* data){
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    unsigned char& loadDataFromShader(){
        if(image != nullptr){
            delete[] image;
            image = nullptr;
        }

        image = new unsigned char[image_width * image_height * 4];
        bind();
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

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
    int image_width;
    int image_height;

    unsigned char* image = nullptr;

};

class TextureList : public AList<Texture>{
private:
    struct TextureLayout{
        void pushTexture(GLint n, size_t textureIndex, string uniformName){
            ns.push_back(n);
            textureIndexes.push_back(textureIndex);
            uniformNames.push_back(uniformName);
            ++layoutSize;
        }

        int layoutSize = 0;
        vector<GLint> ns;
        vector<size_t> textureIndexes;
        vector<string> uniformNames;
    };

    vector<TextureLayout> texLayouts;
public:
    TextureList(string name = "noname"){
        this->name = name;
    }

    void loadNew(string path, string name = "noname"){
        Texture* t = new Texture(name);
        t->loadTexture(path);
        push(*t);
        unbindTextures();
    }
    void pushTextureToShaderByIndex(Shader* shader, GLint n, size_t textureIndex, string uniformName){
        at(textureIndex)->pushToShader(shader, n, uniformName);
    }
    void pushTextureToShaderByName(Shader* shader, GLint n, string textureName, string uniformName){
        getByName(textureName)->pushToShader(shader, n, uniformName);
    }
    void unbindTextures(){
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void appendTextureToLayout(size_t layoutId, GLint n, size_t textureIndex, string uniformName){
        texLayouts.at(layoutId).pushTexture(n, textureIndex, uniformName);
    }
    void addLayouts(int amount){
        for(int i = 0; i < amount; ++i){
            texLayouts.push_back(TextureLayout());
        }
    }
    void pushLayoutToShader(size_t layoutId, Shader* shader){
        TextureLayout* currLayout = &texLayouts.at(layoutId);
        for(size_t i = 0; i < currLayout->textureIndexes.size(); ++i){
            pushTextureToShaderByIndex(shader,
                               currLayout->ns.at(i),
                               currLayout->textureIndexes.at(i),
                               currLayout->uniformNames.at(i));
        }
    }
    size_t layoutsAmount(){
        return texLayouts.size();
    }

    string name;
};
