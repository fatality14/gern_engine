#pragma once

#include <common.h>

#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletonloader.h>
#include <instancedobject.h>
#include <materialloader.h>

#include <unordered_set>
#include <unordered_map>

//must be inheritable to support IFrameModel of Renderer
class SceneLoader : private ALoader{
public:
    ~SceneLoader(){
        for(size_t i = 0; i < meshBuffers.size(); ++i){
            delete meshBuffers[i];
        }
        for(size_t i = 0; i < sklBuffers.size(); ++i){
            delete sklBuffers[i];
        }
        for(size_t i = 0; i < instBuffers.size(); ++i){
            delete instBuffers[i];
        }
        for(size_t i = 0; i < materialLists.size(); ++i){
            delete materialLists[i];
        }
    }

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

        //TODO: check if file is not init to prevent infinie loading

        while(!f.eof()){

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "mesh"){
                tmp1 = cwd + bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                meshList.push(meshLoader.load(tmp1, tmp2));
            }
            if(token == "skl"){
                tmp1 = bite(" ", line, end);
                tmp2 = cwd + bite(" ", line, end);
                skeletonList.push(skeletizer.skeletize(*meshList.getByName(tmp1), tmp2));
            }
            if(token == "shad"){
                tmp1 = cwd + bite(" ", line, end);
                tmp2 = cwd + bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                //vertex, fragment, name
                shaders.pushNew(tmp1, tmp2, tmp3);
            }
            if(token == "meshbuf"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                meshBuffers.push_back(new MeshBuffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
                meshBuffers.at(meshBuffers.size() - 1)->genBuffers();
            }
            if(token == "sklbuf"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                sklBuffers.push_back(new SkeletonBuffer(*skeletonList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
                sklBuffers.at(sklBuffers.size() - 1)->genBuffers();
            }
            if(token == "instbuf"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                instBuffers.push_back(new InstancedBuffer(*meshList.getByName(tmp1), *shaders.getByName(tmp2), tmp3));
                instBuffers.at(instBuffers.size() - 1)->genBuffers();
            }
            if(token == "tex"){
                MaterialList* texbmat = new MaterialList;
                texbmat->pushNew();
                texbmat->name = line;

                materialLists.push_back(texbmat);
            }
            if(token == "texm"){
                materialLists.at(materialLists.size()-1)->textures->loadNew(cwd + line);
            }
            if(token == "texl"){
                materialLists.at(materialLists.size()-1)->textures->addLayouts(1);

                itmp1 = stoi(bite(" ", line, end));
                itmp2 = stoi(bite(" ", line, end));
                itmp3 = stoi(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                materialLists.at(materialLists.size()-1)->textures->appendTextureToLayout(itmp1, itmp2, itmp3, tmp1);
            }

            //fix to use other textures than map_Kd
            if(token == "mat"){
                tmp1 = cwd + bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);

                materialLoader.load(tmp1);
                MaterialList* materials = materialLoader.list;
                TextureList* textures = materials->textures;

                textures->addLayouts(materials->size());//gen layout for each material

                unordered_set<string> loaded;
                unordered_map<string, size_t> ids;
                size_t idCounter = 0;
                for(size_t i = 0; i < materials->size(); ++i){//for every material
                    string texName = materials->at(i)->getTextureNames().at(0);//texture name
                    if(loaded.count(texName) == 0){//if not loaded
                        textures->loadNew(cwd + tmp2 + "\\\\" + texName);//load by path
                        loaded.emplace(texName);//mark loaded
                        ids.emplace(pair(texName, idCounter));//map name to id in TextureList
                        ++idCounter;
                    }
                }

                for(size_t i = 0; i < materials->size(); ++i){
                    string texName = materials->at(i)->getTextureNames().at(0);//texture name
                    textures->appendTextureToLayout(i, 0, ids.at(texName), "texture0");
                }

                materials->name = tmp3;
                materialLists.push_back(materials);
            }
            if(token == "light"){
                float x = stof(bite(" ", line, end));
                float y = stof(bite(" ", line, end));
                float z = stof(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                //x,y,z,name
                renderer.model->addNewLightSource(x, y, z, tmp1);
            }
            if(token == "frmb"){
                itmp1 = stoi(bite(" ", line, end));
                itmp2 = stoi(bite(" ", line, end));
                itmp3 = stoi(bite(" ", line, end));
                tmp1 = bite(" ", line, end);

                Framebuffer* framebuffer = new Framebuffer(itmp1, itmp2, tmp1);
                framebuffer->genTextureColorBuffers(itmp3);
                renderer.model->addFramebuffer(*framebuffer);

                MaterialList* frmbmat = new MaterialList;
                frmbmat->pushNew();

                frmbmat->name = tmp1;
                frmbmat->textures = framebuffer->textureColorBuffers;
                materialLists.push_back(frmbmat);
            }
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
                if(lastSkybox != nullptr){
                    lastSkybox->move(x,y,z);
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
                if(lastSkybox != nullptr){
                    lastSkybox->scaleTo(x,y,z);
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
                if(lastSkybox != nullptr){
                    lastSkybox->rotateTo(x,y,z);
                }
            }
            if(token == "meshobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);

                size_t which1 = 0, which2 = 0;

//                for(size_t i = 0; i < texes.size(); ++i){
//                    if(texes.at(i)->name == tmp1){
//                        which1 = i;
//                        break;
//                    }
//                }

                for(size_t i = 0; i < materialLists.size(); ++i){
                    if(materialLists.at(i)->name == tmp1){
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
                //remove textures from addNewObject cause it moved it Materials
                renderer.model->addNewObject(*meshBuffers.at(which2),
                                      materialLists.at(which1), tmp4);
                lastMeshObj = renderer.model->getMeshObject(tmp4);
                lastSklObj = nullptr;
                lastSkybox = nullptr;
            }
            if(token == "sklobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);
                tmp5 = cwd + bite(" ", line, end);
                itmp1 = stoi(bite(" ", line, end));

                size_t which1 = 0, which2 = 0;

//                for(size_t i = 0; i < texes.size(); ++i){
//                    if(texes.at(i)->name == tmp1){
//                        which1 = i;
//                        break;
//                    }
//                }

                for(size_t i = 0; i < materialLists.size(); ++i){
                    if(materialLists.at(i)->name == tmp1){
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

                renderer.model->addNewObject(*sklBuffers.at(which2),
                                              materialLists.at(which1), tmp4);
                renderer.model->getSkeletonObject(tmp4)->parseAndPushAnimation(tmp5, itmp1, tmp5);//add name instead of last arg
                renderer.model->getSkeletonObject(tmp4)->setCurrAnimation(tmp5);
                renderer.model->getSkeletonObject(tmp4)->startAnimation();
                lastSklObj = renderer.model->getSkeletonObject(tmp4);
                lastMeshObj = nullptr;
                lastSkybox = nullptr;
            }
            if(token == "instobj"){
                tmp1 = bite(" ", line, end);
                tmp2 = bite(" ", line, end);
                tmp3 = bite(" ", line, end);
                tmp4 = bite(" ", line, end);
                tmp5 = bite(" ", line, end);

                size_t which1 = 0, which2 = 0;

//                for(size_t i = 0; i < texes.size(); ++i){
//                    if(texes.at(i)->name == tmp1){
//                        which1 = i;
//                        break;
//                    }
//                }

                for(size_t i = 0; i < materialLists.size(); ++i){
                    if(materialLists.at(i)->name == tmp1){
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

                renderer.model->addNewObject(*instBuffers.at(which2), materialLists.at(which1), poses, tmp5);
            }
            if(token == "skybox"){
                vector<string> skyboxSides;
                skyboxSides.push_back(cwd + bite(" ", line, end));
                skyboxSides.push_back(cwd + bite(" ", line, end));
                skyboxSides.push_back(cwd + bite(" ", line, end));
                skyboxSides.push_back(cwd + bite(" ", line, end));
                skyboxSides.push_back(cwd + bite(" ", line, end));
                skyboxSides.push_back(cwd + bite(" ", line, end));

                tmp4 = bite(" ", line, end);

                size_t which = 0;

                for(size_t i = 0; i < sklBuffers.size(); ++i){
                    if(sklBuffers.at(i)->name == tmp2){
                        which = i;
                        break;
                    }
                }

                renderer.model->addNewSkybox(skyboxSides, *meshBuffers.at(which), tmp4);

                lastSkybox = renderer.model->getSkyboxObject(tmp4);
                lastSklObj = nullptr;
                lastMeshObj = nullptr;
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
    SkeletonLoader skeletizer;
    MaterialLoader materialLoader;
    ShaderList shaders;
    vector<MaterialList*> materialLists;

    MeshObject* lastMeshObj;
    SkeletonObject* lastSklObj;
    SkyboxObject* lastSkybox;

    vector<MeshBuffer*> meshBuffers;
    vector<SkeletonBuffer*> sklBuffers;
    vector<InstancedBuffer*> instBuffers;
};
