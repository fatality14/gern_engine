#pragma once

#include <buffer/meshbuffer.h>
#include <object/aobject.h>
#include <shader/lightsource.h>
#include <texture/material.h>

class MeshObject : public AObject<MeshBuffer> {
public:
    LightSourceList* lightSources;
    MaterialList* materials;
    TextureList* texList;

    // make arguments optional
    MeshObject(Window& w, MeshBuffer& b, Perspective& p, View& v,
               LightSourceList& lsl, MaterialList& ml,
               const string& name = "noname")
        : AObject(w, b, p, v, name) {
        lightSources = &lsl;
        materials = &ml;
        texList = materials->textures;

        shaderFields.push(*position);
        shaderFields.push(*perspective);
        shaderFields.push(*view);
        shaderFields.push(*lightSources);
    }

    void setTextureList(TextureList& tl) { texList = &tl; }
    TextureList& getTextureList() { return *texList; }

    void draw(int flags = 0) override {
        shader->bind();

        position->setDefaultEvents(*window);

        if (currShaderId != shader->program) {
            perspective->pushToShader(*shader);
            view->pushToShader(*shader);
            lightSources->pushToShader(*shader);

            currShaderId = shader->program;
        }
        if (flags == 0) {
            shaderFields.pushToShader(*shader);
        }

        buffer->bind();

        Mesh* currMesh = &buffer->getMesh();
        // cout << "Draw mesh: " << currMesh->name << endl;
        GLint startFrom = 0;
        size_t textureI = 0;
        size_t materialI = 0;

        for (size_t j = 0; j < currMesh->partEndMtlIds.size(); ++j) {
            if (textureI == texList->layoutsAmount()) {
                textureI = 0;
            }
            texList->setShaderParams(textureI);
            texList->pushToShader(*shader);
            ++textureI;

            if (materialI == materials->size()) {
                materialI = 0;
            }
            materials->at(materialI)->pushToShader(*shader);
            ++materialI;

            if (j == 0) {
                GLDB(glDrawArrays(drawmode, startFrom,
                                  currMesh->partEndMtlIds.at(j)));
                startFrom = currMesh->partEndMtlIds.at(j);
            } else {
                GLDB(glDrawArrays(drawmode, startFrom,
                                  currMesh->partEndMtlIds.at(j) -
                                      currMesh->partEndMtlIds.at(j - 1)));
                startFrom = currMesh->partEndMtlIds.at(j);
            }
        }

        buffer->unbind();
        shader->unbind();
        texList->unbindTextures();
    }

    inline static GLuint currShaderId = -1;
};

class MeshObjectList : public AListO<MeshObject> {};
