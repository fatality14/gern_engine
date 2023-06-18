#pragma once

#include <common/common.h>
#include <common/loader.h>
#include <texture/material.h>

class MaterialLoader : private ULoader {
public:

    MaterialList* load(const filesystem::path& path) {
        MaterialList* list = new MaterialList;

        ifstream f;
        f.open(path);

        if (f.fail()) {
            throw string("Cannot open file: ") + path.string();
        }

        string line;

        getline(f, line);
        removeExcessSpaces(line);

        string token = bite(" ", line);

        string tmp;
        float ftmp1, ftmp2, ftmp3;

        // TODO: check if file is not init to prevent infinie loading

        while (!f.eof()) {
            Material* m;

            getline(f, line);
            removeExcessSpaces(line);

            token = bite(" ", line);

            if (token == "#") {
                continue;
            }
            if (token == "newmtl") {
                m = new Material(bite(" ", line));
                list->push(*m);
            }
            if (token == "Ka") {
                ftmp1 = biteFloat(" ", line);
                ftmp2 = biteFloat(" ", line);
                ftmp3 = biteFloat(" ", line);
                m->setAmbientColor(ftmp1, ftmp2, ftmp3);
            }
            if (token == "Kd") {
                ftmp1 = biteFloat(" ", line);
                ftmp2 = biteFloat(" ", line);
                ftmp3 = biteFloat(" ", line);
                m->setDiffuseColor(ftmp1, ftmp2, ftmp3);
            }
            if (token == "Ks") {
                ftmp1 = biteFloat(" ", line);
                ftmp2 = biteFloat(" ", line);
                ftmp3 = biteFloat(" ", line);
                m->setSpecularColor(ftmp1, ftmp2, ftmp3);
            }
            if (token == "Ns") {
                ftmp1 = biteFloat(" ", line);
                m->setSpecularHighlights(ftmp1);
            }
            if (token == "Ni") {
                ftmp1 = biteFloat(" ", line);
                m->setOpticalDensity(ftmp1);
            }
            if (token == "d") {
                ftmp1 = biteFloat(" ", line);
                m->setDissolve(ftmp1);
            }
            if (token == "map_Kd") {
                m->pushTextureName(bite(" ", line));
            }
            if (token == "map_Ka") {
                m->pushTextureName(bite(" ", line));
            }
            //...other maps
        }

        return list;
    }
};
