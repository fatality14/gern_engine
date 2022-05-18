#pragma once

#include <common/common.h>
#include <shader/shaderfield.h>
#include <texture/texture.h>

class Material : public IShaderField{
public:
    Material(string matname = "noname"){
        this->matname = matname;
    }

    void setShaderParams(string name){
        this->name = name;
    }
    void pushToShader(Shader& s) override{
        s.setUniform3fv(name + ".ambientColor", glm::value_ptr(ambientColor));
        s.setUniform3fv(name + ".diffuseColor", glm::value_ptr(diffuseColor));
        s.setUniform3fv(name + ".specularColor", glm::value_ptr(specularColor));

        s.setUniform1f(name + ".specularHighlights", specularHighlights);
        s.setUniform1f(name + ".opticalDensity", opticalDensity);
        s.setUniform1f(name + ".dissolve", dissolve);
    }

    void setAmbientColor(float r, float g, float b){
        ambientColor = glm::vec3(r,g,b);
    }
    void setDiffuseColor(float r, float g, float b){
        diffuseColor = glm::vec3(r,g,b);
    }
    void setSpecularColor(float r, float g, float b){
        specularColor = glm::vec3(r,g,b);
    }

    void setSpecularHighlights(float specularHighlights){
        this->specularHighlights = specularHighlights;
    }
    void setOpticalDensity(float density){
        opticalDensity = density;
    }
    void setDissolve(float dissolve){
        this->dissolve = dissolve;
    }

    glm::vec3 getAmbientColor(){
        return ambientColor;
    }
    glm::vec3 getDiffuseColor(){
        return diffuseColor;
    }
    glm::vec3 getSpecularColor(){
        return specularColor;
    }

    float getSpecularHighlights(){
        return specularHighlights;
    }
    float getOpticalDensity(){
        return opticalDensity;
    }
    float getDissolve(){
        return dissolve;
    }

    void pushTextureName(string t){
        textureNames.push_back(t);
    }
    vector<string> getTextureNames(){
        return textureNames;
    }

    string matname;
private:
    glm::vec3 ambientColor = glm::vec3(1.f);
    glm::vec3 diffuseColor = glm::vec3(1.f);
    glm::vec3 specularColor = glm::vec3(1.f);

    float specularHighlights = 1.f;
    float opticalDensity = 1.f;
    float dissolve = 1.f;
    vector<string> textureNames;
};

class MaterialList : public AListO<Material>{
public:
    void pushNew(string matname = "noname"){
        push(*(new Material(matname)));
    }

    TextureList* textures = new TextureList;
    string name = "noname";
};
