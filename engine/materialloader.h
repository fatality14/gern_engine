#pragma once

#include <common.h>
#include <material.h>

class MaterialLoader : private ULoader{
public:
    MaterialList* list;

    void load(string path){
        list = new MaterialList;

        ifstream f;
        f.open(path);

        string line;
        bool end;

        getline(f, line);
        removeBadSpaces(line);

        string token = bite(" ", line, end);

        string tmp;
        float ftmp1, ftmp2, ftmp3;

        //TODO: check if file is not init to prevent infinie loading

        while(!f.eof()){
            Material* m;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);


            if(token == "#"){
                continue;
            }
            if(token == "newmtl"){
                m = new Material(bite(" ", line, end));
                list->push(*m);
            }
            if(token == "Ka"){
                ftmp1 = stof(bite(" ", line, end));
                ftmp2 = stof(bite(" ", line, end));
                ftmp3 = stof(bite(" ", line, end));
                m->setAmbientColor(ftmp1, ftmp2, ftmp3);
            }
            if(token == "Kd"){
                ftmp1 = stof(bite(" ", line, end));
                ftmp2 = stof(bite(" ", line, end));
                ftmp3 = stof(bite(" ", line, end));
                m->setDiffuseColor(ftmp1, ftmp2, ftmp3);
            }
            if(token == "Ks"){
                ftmp1 = stof(bite(" ", line, end));
                ftmp2 = stof(bite(" ", line, end));
                ftmp3 = stof(bite(" ", line, end));
                m->setSpecularColor(ftmp1, ftmp2, ftmp3);
            }
            if(token == "Ns"){
                ftmp1 = stof(bite(" ", line, end));
                m->setSpecularHighlights(ftmp1);
            }
            if(token == "Ni"){
                ftmp1 = stof(bite(" ", line, end));
                m->setOpticalDensity(ftmp1);
            }
            if(token == "d"){
                ftmp1 = stof(bite(" ", line, end));
                m->setDissolve(ftmp1);
            }
            if(token == "map_Kd"){
                m->pushTextureName(bite(" ", line, end));
            }
            if(token == "map_Ka"){
                m->pushTextureName(bite(" ", line, end));
            }
            //...other maps
        }
    }
};
