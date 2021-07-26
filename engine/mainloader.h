#pragma once

#include <common.h>

#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletizer.h>
#include <instancedobject.h>

class  MainLoader : private Loader{
public:
    void load(string path, Renderer renderer){
        ifstream f;
        f.open(path);

        string line;
        int switchCase;
        bool end;
        string cwd;

        getline(f, line);
        removeBadSpaces(line);

        token = bite(" ", line, end);

        if(token == "cwd"){
            switchCase = 0;
        }

        cwd = line;

        string tmp1, tmp2, tmp3, tmp4;
        size_t itmp1, itmp2, itmp3;

        if(cwd != ""){
            while(!f.eof()){
                switchCase = -1;

                getline(f, line);
                removeBadSpaces(line);

                token = bite(" ", line, end);

                if(token == "mesh"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    meshList.push(meshLoader.load(cwd + tmp1, tmp2));
                }
                if(token == "skl"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    skeletonList.push(skeletizer.skeletize(*meshList.getByName(tmp2), cwd + tmp1));
                }
                if(token == "shad"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    tmp3 = bite(" ", line, end);
                    //vertex, fragment, name
                    shaders.pushNew(tmp1, tmp2, tmp3);
                }
                if(token == "tex"){
                    texes.push_back(TextureList(line));
                }
                if(token == "texm"){
                    texes.end()->loadNew(line);
                }
                if(token == "texl"){
                    texes.end()->addLayouts(1);

                    itmp1 = stoi(bite(" ", line, end));
                    itmp2 = stoi(bite(" ", line, end));
                    itmp3 = stoi(bite(" ", line, end));
                    tmp1 = bite(" ", line, end);

                    texes.end()->appendTextureToLayout(itmp1, itmp2, itmp3, tmp4);
                    /*bool end;

                    while (true){
                        token = bite(" ", line, end);
                        keyPoses.at(pAmount).ids.push_back(stoi(token));
                        if(end)
                            break;
                    }*/
                }
                if(token == "meshbuf"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    meshBuffers.push_back(Buffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2)));
                }
                if(token == "sklbuf"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    sklBuffers.push_back(SkeletonBuffer(*skeletonList.getByName(tmp1), *shaders.getByName(tmp2)));
                }
                if(token == "instbuf"){
                    tmp1 = bite(" ", line, end);
                    tmp2 = bite(" ", line, end);
                    instBuffers.push_back(InstancedBuffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2)));
                }
                if(token == "mat"){
                    //TODO: replace this with good code
                    Material* mat = new Material;
                    mat->setAmbientColor(0.01f,0.01f,0.01f);
                    mat->setSpecularHighlights(20);

                    materials.push(*mat);
                }
                if(token == "light"){
                    itmp1 = stoi(bite(" ", line, end));
                    itmp2 = stoi(bite(" ", line, end));
                    itmp3 = stoi(bite(" ", line, end));
                    tmp1 = bite(" ", line, end);

                    //x,y,z,name
                    renderer.addNewLightSource(itmp1, itmp2, itmp3, tmp1);
                }
                if(token == "frmb"){
                    itmp1 = stoi(bite(" ", line, end));
                    itmp2 = stoi(bite(" ", line, end));
                    itmp3 = stoi(bite(" ", line, end));

                    Framebuffer* framebuffer = new Framebuffer(itmp1, itmp2);
                    framebuffer->genTextureColorBuffers(itmp3);
                    renderer.addFramebuffer(*framebuffer);
                }
                if(token == "meshobj"){
                    switchCase = 9;
                }
                if(token == "sklobj"){
                    switchCase = 10;
                }
                if(token == "instobj"){
                    switchCase = 11;
                }
                if(token == "skybox"){
                    switchCase = 12;
                }
                if(token == "bckcol"){
                    switchCase = 13;
                }
            }
        }
    }
private:
    MeshLoader meshLoader;
    MeshList meshList;
    SkeletonMeshList skeletonList;
    Skeletizer skeletizer;
    ShaderList shaders;
    vector<TextureList> texes;
    MaterialList materials;

    vector<Buffer> meshBuffers;
    vector<SkeletonBuffer> sklBuffers;
    vector<InstancedBuffer> instBuffers;
};
