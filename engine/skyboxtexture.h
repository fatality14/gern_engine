#pragma once

#include <common.h>
#include <shaderfield.h>

class SkyboxTexture : public ITexture, public IShaderField{
public:
    GLuint textureId;
    vector<string> facePaths;
    SkyboxTexture(vector<string> facePaths){
        this->facePaths = facePaths;

        loadTexture();
    }
    SkyboxTexture(SkyboxTexture& st) = delete;
    ~SkyboxTexture(){
        glDeleteTextures(1, &textureId);
    }

    void setShaderParams(GLint n, string uniformName){
        this->n = n;
        this->name = uniformName;
    }
    void pushToShader(Shader& shader) override{
        shader.setUniform1i(name, n);

        glActiveTexture(GL_TEXTURE0 + n);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    }

    void loadTexture() override{
        //load image
        image_width = 0;
        image_height = 0;

        //init and active texture
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

        for(size_t i = 0; i < facePaths.size(); i++){
            image = SOIL_load_image(facePaths.at(i).data(), &image_width, &image_height, NULL, SOIL_LOAD_RGB);
            if(image){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                SOIL_free_image_data(image);
            }
            else{
                cout << "Texture loading failed" << endl;
            }
        }

        //setup texture
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
private:
    int image_width;
    int image_height;

    GLint n = 0;

    unsigned char* image;
};
