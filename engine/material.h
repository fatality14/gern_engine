#pragma once

#include <common.h>
#include <shader.h>

class Material{
public:
    void pushToShader(Shader* s, string uniformName){
        s->setUniform3fv(uniformName + ".ambientColor", glm::value_ptr(ambientColor));
        s->setUniform3fv(uniformName + ".diffuseColor", glm::value_ptr(diffuseColor));
        s->setUniform3fv(uniformName + ".specularColor", glm::value_ptr(specularColor));

        s->setUniform1f(uniformName + ".specularHighlights", specularHighlights);
        s->setUniform1f(uniformName + ".opticalDensity", opticalDensity);
        s->setUniform1f(uniformName + ".dissolve", dissolve);
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
    void setOticalDensity(float density){
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
private:
    glm::vec3 ambientColor = glm::vec3(1.f);
    glm::vec3 diffuseColor = glm::vec3(1.f);
    glm::vec3 specularColor = glm::vec3(1.f);

    float specularHighlights = 1.f;
    float opticalDensity = 1.f;
    float dissolve = 1.f;
    //Texture* colorTexture;
};

class MaterialList : public List<Material>{

};
