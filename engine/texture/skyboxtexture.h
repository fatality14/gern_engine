#pragma once

#include <common/common.h>
#include <shader/ishaderfield.h>
#include <texture/itexture.h>

class SkyboxTexture : public ITexture, public IShaderField {
public:
    GLuint textureId;
    vector<string> facePaths;
    SkyboxTexture(vector<string> facePaths) {
        this->facePaths = facePaths;

        loadTexture();
    }
    SkyboxTexture(SkyboxTexture& st) = delete;
    ~SkyboxTexture() { GLDB(glDeleteTextures(1, &textureId)); }

    void setShaderParams(GLuint textureUnit, string uniformName) {
        this->textureUnit = textureUnit;
        this->name = uniformName;
    }
    void pushToShader(Shader& shader) override {
        shader.setUniform1i(name, textureUnit);

        GLDB(glActiveTexture(GL_TEXTURE0 + textureUnit));
        GLDB(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));
    }

    void loadTexture() override {
        image_width = 0;
        image_height = 0;

        GLDB(glGenTextures(1, &textureId));
        GLDB(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));

        for (size_t i = 0; i < facePaths.size(); i++) {
            image = SOIL_load_image(facePaths.at(i).data(), &image_width,
                                    &image_height, NULL, SOIL_LOAD_RGB);
            if (image) {
                GLDB(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                                  image_width, image_height, 0, GL_RGB,
                                  GL_UNSIGNED_BYTE, image));
                SOIL_free_image_data(image);
            } else {
                throw string("Texture ") + facePaths.at(i) + " loading failed";
            }
        }

        GLDB(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                             GL_LINEAR));
        GLDB(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,
                             GL_LINEAR));
        GLDB(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                             GL_CLAMP_TO_EDGE));
        GLDB(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                             GL_CLAMP_TO_EDGE));
        GLDB(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                             GL_CLAMP_TO_EDGE));
    }

private:
    GLsizei image_width;
    GLsizei image_height;

    GLuint textureUnit = 0;

    unsigned char* image;
};
