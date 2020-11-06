#pragma once

#include <engine/common.h>
#include <engine/shader.h>

class Texture{
public:
    GLuint textureId;
    string name;
    string path;

    Texture(string name = "noname"){
        this->path = "";
        this->name = name;
        glGenTextures(1, &textureId);
    }
    Texture(string path, string name = "noname"){
        this->path = path;
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
    void loadTexture(){
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
        //mipmap is used to compress texture or smthg like this
        if(image){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            cout << "Texture loading failed" << endl;
        }

        //free space
        SOIL_free_image_data(image);
    }
private:
    int image_width;
    int image_height;

    unsigned char* image;

};

class TextureList : public List<Texture>{
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
    void loadNew(string path, string name = "noname"){
        Texture* t = new Texture(path, name);
        t->loadTexture();
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
};
