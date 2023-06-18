#pragma once

#include <array>
#include <common/common.h>
#include <memory>
#include <shader/ishaderfield.h>
#include <texture/itexture.h>

class SkyboxTexture : public ITexture, public IShaderField {
public:
    GLuint textureId;
    array<filesystem::path, 6> facePaths;
    SkyboxTexture(const array<filesystem::path, 6>& facePaths) {
        this->facePaths = facePaths;

        loadTexture();
    }
    SkyboxTexture(SkyboxTexture& st) = delete;
    ~SkyboxTexture() { GLDB(glDeleteTextures(1, &textureId)); }

    void setShaderParams(GLuint textureUnit, const string& uniformName) {
        this->textureUnit = textureUnit;
        this->name = uniformName;
    }
    void pushToShader(Shader& shader) override {
        shader.setUniform1i(name, textureUnit);

        GLDB(glActiveTexture(GL_TEXTURE0 + textureUnit));
        GLDB(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));
    }

    void loadTexture() override {
        GLsizei image_width = 0;
        GLsizei image_height = 0;

        GLDB(glGenTextures(1, &textureId));
        GLDB(glBindTexture(GL_TEXTURE_CUBE_MAP, textureId));

        for (size_t i = 0; i < facePaths.size(); i++) {
            unique_ptr<unsigned char[]> image (SOIL_load_image(facePaths.at(i).c_str(), &image_width,
                                    &image_height, NULL, SOIL_LOAD_RGB));
            if (image) {
                GLDB(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
                                  image_width, image_height, 0, GL_RGB,
                                  GL_UNSIGNED_BYTE, image.get()));
                // SOIL_free_image_data(image); //no need to with unique_ptr
            } else {
                throw string("Texture ") + facePaths.at(i).string() + " loading failed";
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
    GLuint textureUnit = 0;
};
