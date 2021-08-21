#pragma once

#include <common.h>

#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletizer.h>
#include <instancedobject.h>

class  MainLoader : private Loader{
public:
    void load(string path, Renderer& renderer){
        ifstream f;
        f.open(path);

        string line;
        bool end;
        string cwd;

        getline(f, line);
        removeBadSpaces(line);

        token = bite(" ", line, end);
        cwd = line;

        string tmp1, tmp2, tmp3, tmp4, tmp5;
        size_t itmp1, itmp2, itmp3;

        while(!f.eof()){

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "mesh"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                meshList.push(meshLoader.load(tmp1, tmp2));
            }
            if(token == "skl"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                skeletonList.push(skeletizer.skeletize(*meshList.getByName(tmp1), tmp2));
            }
            if(token == "shad"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                //vertex, fragment, name
                shaders.pushNew(tmp1, tmp2, tmp3);
            }
            if(token == "tex"){
                texes.push_back(new TextureList(line));
            }
            if(token == "texm"){
                texes.at(texes.size()-1)->loadNew(line);
            }
            if(token == "texl"){
                texes.at(texes.size()-1)->addLayouts(1);

                itmp1 = stoi(bite(" ", line, end));
                itmp2 = stoi(bite(" ", line, end));
                itmp3 = stoi(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                texes.at(texes.size()-1)->appendTextureToLayout(itmp1, itmp2, itmp3, tmp1);
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
                tmp3 = bite(" ", line, end);
                meshBuffers.push_back(new Buffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
            }
            if(token == "sklbuf"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                sklBuffers.push_back(new SkeletonBuffer(*skeletonList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
            }
            if(token == "instbuf"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                instBuffers.push_back(new InstancedBuffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
            }
            if(token == "mat"){
                //TODO: replace this with good code
                Material* mat = new Material;
                mat->setAmbientColor(0.01f,0.01f,0.01f);
                mat->setSpecularHighlights(20);

                materials.push(*mat);
            }
            if(token == "light"){
                float x = stof(bite(" ", line, end));
                float y = stof(bite(" ", line, end));
                float z = stof(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                //x,y,z,name
                renderer.addNewLightSource(x, y, z, tmp1);
            }
            if(token == "frmb"){
                itmp1 = stoi(bite(" ", line, end));
                itmp2 = stoi(bite(" ", line, end));
                itmp3 = stoi(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                Framebuffer* framebuffer = new Framebuffer(itmp1, itmp2, tmp1);
                framebuffer->genTextureColorBuffers(itmp3);
                renderer.addFramebuffer(*framebuffer);

                texes.push_back(framebuffer->textureColorBuffers);
            }
            //make these apply to last added obj
            if(token == "move"){
                float x = stof(bite(" ", line, end));
                float y = stof(bite(" ", line, end));
                float z = stof(bite(" ", line, end));

                if(lastMeshObj != nullptr){
                    lastMeshObj->move(x,y,z);
                }
                if(lastSklObj != nullptr){
                    lastSklObj->move(x,y,z);
                }
            }
            if(token == "scale"){
                float x = stof(bite(" ", line, end));
                float y = stof(bite(" ", line, end));
                float z = stof(bite(" ", line, end));

                if(lastMeshObj != nullptr){
                    lastMeshObj->scaleTo(x,y,z);
                }
                if(lastSklObj != nullptr){
                    lastSklObj->scaleTo(x,y,z);
                }
            }
            if(token == "rot"){
                float x = stof(bite(" ", line, end));
                float y = stof(bite(" ", line, end));
                float z = stof(bite(" ", line, end));

                if(lastMeshObj != nullptr){
                    lastMeshObj->rotateTo(x,y,z);
                }
                if(lastSklObj != nullptr){
                    lastSklObj->rotateTo(x,y,z);
                }
            }

            if(token == "meshobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);

                size_t which1 = 0, which2 = 0;

                for(size_t i = 0; i < texes.size(); ++i){
                    if(texes.at(i)->name == tmp1){
                        which1 = i;
                        break;
                    }
                }

                for(size_t i = 0; i < meshBuffers.size(); ++i){
                    if(meshBuffers.at(i)->name == tmp2){
                        which2 = i;
                        break;
                    }
                }

                renderer.addNewObject(*texes.at(which1), *meshBuffers.at(which2), &materials, tmp4);
                lastMeshObj = renderer.getObjectByName(tmp4);
                lastSklObj = nullptr;
            }
            if(token == "sklobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);
                tmp5 = bite(" ", line, end);
                itmp1 = stoi(bite(" ", line, end));

                size_t which1 = 0, which2 = 0;

                for(size_t i = 0; i < texes.size(); ++i){
                    if(texes.at(i)->name == tmp1){
                        which1 = i;
                        break;
                    }
                }

                for(size_t i = 0; i < sklBuffers.size(); ++i){
                    if(sklBuffers.at(i)->name == tmp2){
                        which2 = i;
                        break;
                    }
                }

                renderer.addNewSkeletonObject(*texes.at(which1), *sklBuffers.at(which2), &materials, tmp4);
                renderer.getSkeletonObjectByName(tmp4)->setAnimation(tmp5, itmp1);
                renderer.getSkeletonObjectByName(tmp4)->startAnimation();
                lastSklObj = renderer.getSkeletonObjectByName(tmp4);
                lastMeshObj = nullptr;

            }
            if(token == "instobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);

                size_t which1 = 0, which2 = 0;

                for(size_t i = 0; i < texes.size(); ++i){
                    if(texes.at(i)->name == tmp1){
                        which1 = i;
                        break;
                    }
                }

                for(size_t i = 0; i < instBuffers.size(); ++i){
                    if(instBuffers.at(i)->name == tmp2){
                        which2 = i;
                        break;
                    }
                }

                vector<Position> poses;
                Position pos;
                pos.scaleTo(0.1,0.1,0.1);
                float rad = 5;
                int am = 30;
                float shift = 360/am;
                float angle = 0;
                for(int i = 0; i < am; ++i){
                    pos.moveTo(rad*sin(angle),rad*cos(angle),0);
                    poses.push_back(pos);
                    angle += shift;
                }

                renderer.addNewInstancedObject(*texes.at(which1), *instBuffers.at(which2), &materials, poses, tmp4);
            }
            if(token == "skybox"){
                vector<string> skyboxSides;
                skyboxSides.push_back(bite(" ", line, end));
                skyboxSides.push_back(bite(" ", line, end));
                skyboxSides.push_back(bite(" ", line, end));
                skyboxSides.push_back(bite(" ", line, end));
                skyboxSides.push_back(bite(" ", line, end));
                skyboxSides.push_back(bite(" ", line, end));

                size_t which = 0;

                for(size_t i = 0; i < sklBuffers.size(); ++i){
                    if(sklBuffers.at(i)->name == tmp2){
                        which = i;
                        break;
                    }
                }

                renderer.addNewSkybox(skyboxSides, *meshBuffers.at(which));
                renderer.getSkyboxObjectByIndex(0)->scaleTo(10,10,10);
            }
            if(token == "bckcol"){
                float r, g, b, a;

                r = stof(bite(" ", line, end));
                g = stof(bite(" ", line, end));
                b = stof(bite(" ", line, end));
                a = stof(bite(" ", line, end));

                renderer.setBackgroundColor(r, g, b, a);
            }
        }
    }
private:
    MeshLoader meshLoader;
    MeshList meshList;
    SkeletonMeshList skeletonList;
    Skeletizer skeletizer;
    ShaderList shaders;
    vector<TextureList*> texes;
    MaterialList materials;

    Object* lastMeshObj;
    SkeletonObject* lastSklObj;

    vector<Buffer*> meshBuffers;
    vector<SkeletonBuffer*> sklBuffers;
    vector<InstancedBuffer*> instBuffers;
};
