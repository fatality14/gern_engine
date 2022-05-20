#pragma once

#include <common/common.h>

#include <render/framemodel.h>
#include <mesh/meshloader.h>
#include <object/skeletonobject.h>
#include <mesh/skeletonloader.h>
#include <object/instancedobject.h>
#include <texture/materialloader.h>

#include <unordered_set>
#include <unordered_map>
#include <queue>

class IContext : public ICommon{};

template <class U>
class ICommand : public ICommon{
public:
    ICommand(){
       static_assert(std::is_base_of<IContext, U>::value, "Template parameter U must be derived from IContext");
    }
    virtual void execute(U& context) = 0;
};

template <class T>
class ISceneLoader : public ICommon{
public:
    ISceneLoader(){
       static_assert(std::is_base_of<IFrameModel, T>::value, "Template parameter T must be derived from IFrameModel");
    }
    virtual void load(string path, T& data) = 0;
};

class SceneLoader;

class LoaderContext : public IContext{
public:
    ~LoaderContext(){
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

    bool step(){
        if(lines.size() != 0){
            string& currLine = lines.front();

            ULoader::removeBadSpaces(currLine);

            command = ULoader::bite(" ", currLine);
            args = currLine;

            lines.pop();

            return true;
        }
        else{
            return false;
        }
    }

    string getNextCommand(){
        string nextCommand = lines.front();
        return ULoader::bite(" ", nextCommand);
    }

    string cwd;

    FrameModel* model;
    queue<string> lines;
    string args;
    string command;

    MeshLoader meshLoader;
    MeshList meshList;
    SkeletonMeshList skeletonList;
    SkeletonLoader skeletizer;
    MaterialLoader materialLoader;
    ShaderList shaders;
    vector<MaterialList*> materialLists;

    vector<MeshBuffer*> meshBuffers;
    vector<SkeletonBuffer*> sklBuffers;
    vector<InstancedBuffer*> instBuffers;
};

typedef ICommand<LoaderContext> Command;

class SceneLoader : private ULoader, public ISceneLoader<FrameModel>{
public:
    void load(string path, FrameModel& data) override {
        c.model = &data;

        ifstream f;
        f.open(path);

        string line;

        if(f.fail()){
            throw string("Cannot open file: ") + path;
        }

        while(!f.eof()){
            getline(f, line);
            removeBadSpaces(line);
            c.lines.push(line);
        }

        c.step();
        if(c.command == "cwd"){
            cwdc.execute(c);
        }

        while(c.step()){
            if(c.command == "mesh"){
                meshc.execute(c);
                continue;
            }
            if(c.command == "skl"){
                sklc.execute(c);
                continue;
            }
            if(c.command == "shad"){
                shadc.execute(c);
                continue;
            }
            if(c.command == "meshbuf"){
                meshbufc.execute(c);
                continue;
            }
            if(c.command == "sklbuf"){
                sklbufc.execute(c);
                continue;
            }
            if(c.command == "instbuf"){
                instbufc.execute(c);
                continue;
            }
            if(c.command == "tex"){
                texc.execute(c);
                continue;
            }
            //fix to use other textures than map_Kd
            if(c.command == "mat"){
                matc.execute(c);
                continue;
            }
            if(c.command == "light"){
                lightc.execute(c);
                continue;
            }
            if(c.command == "cam"){
                camc.execute(c);
                continue;
            }
            if(c.command == "frmb"){
                frmbc.execute(c);
                continue;
            }
            if(c.command == "meshobj"){
                meshobjc.execute(c);
                continue;
            }
            if(c.command == "sklobj"){
                sklobjc.execute(c);
                continue;
            }
            if(c.command == "instobj"){
                instobjc.execute(c);
                continue;
            }
            if(c.command == "skybox"){
                skyboxc.execute(c);
                continue;
            }
            if(c.command == "bckcol"){
                bckcolc.execute(c);
                continue;
            }
        }
    }

    LoaderContext c;
private:    
    class CwdCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            c.cwd = c.args;
        }
    };

    class MeshCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = c.cwd + bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            c.meshList.push(c.meshLoader.load(tmp1, tmp2));
        }
    };

    class SklCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = c.cwd + bite(" ", c.args);
            c.skeletonList.push(c.skeletizer.skeletize(*(c.meshList).getByName(tmp1), tmp2));
        }
    };

    class ShadCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = c.cwd + bite(" ", c.args);
            string tmp2 = c.cwd + bite(" ", c.args);
            string tmp3 = bite(" ", c.args);
            //vertex, fragment, name
            c.shaders.pushNew(tmp1, tmp2, tmp3);
        }
    };

    class MeshBufCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp3 = bite(" ", c.args);
            c.meshBuffers.push_back(new MeshBuffer(*(c.meshList).getByName(tmp1), *(c.shaders).getByName(tmp2), tmp3));
            c.meshBuffers.at(c.meshBuffers.size() - 1)->genBuffers();
        }
    };

    class SklBufCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp3 = bite(" ", c.args);
            c.sklBuffers.push_back(new SkeletonBuffer(*(c.skeletonList).getByName(tmp1), *(c.shaders).getByName(tmp2), tmp3));
            c.sklBuffers.at(c.sklBuffers.size() - 1)->genBuffers();
        }
    };

    class InstBufCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp3 = bite(" ", c.args);
            c.instBuffers.push_back(new InstancedBuffer(*(c.meshList).getByName(tmp1), *(c.shaders).getByName(tmp2), tmp3));
            c.instBuffers.at(c.instBuffers.size() - 1)->genBuffers();
        }
    };

    class TexLCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            c.materialLists.at(c.materialLists.size()-1)->textures->addLayouts(1);

            size_t itmp1 = biteInt(" ", c.args);
            size_t itmp2 = biteInt(" ", c.args);
            size_t itmp3 = biteInt(" ", c.args);
            string tmp1 = bite(" ", c.args);

            c.materialLists.at(c.materialLists.size()-1)->textures->appendTextureToLayout(itmp1, itmp2, itmp3, tmp1);

            string nextCommand = c.getNextCommand();
            if(nextCommand == "texl"){
                c.step();
                texlc.execute(c);
            }
        }
    };

    class TexMCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            c.materialLists.at(c.materialLists.size()-1)->textures->loadNew(c.cwd + c.args);

            c.step();
            if(c.command == "texm"){
                texmc.execute(c);
            }
            else if(c.command == "texl"){
                texlc.execute(c);
            }
            else{
                throw string("there must be a texl command after texm block in scene");
            }
        }
    };

    class TexCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            MaterialList* texbmat = new MaterialList;
            texbmat->pushNew();
            texbmat->name = c.args;

            c.materialLists.push_back(texbmat);

            c.step();
            if(c.command == "texm"){
                texmc.execute(c);
            }
            else{
                throw string("there must be a texm command after tex in scene");
            }
        }
    };

    class MatCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = c.cwd + bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp3 = bite(" ", c.args);

            c.materialLoader.load(tmp1);
            MaterialList* materials = c.materialLoader.list;
            TextureList* textures = materials->textures;

            textures->addLayouts(materials->size());//gen layout for each material

            unordered_set<string> loaded;
            unordered_map<string, size_t> ids;
            size_t idCounter = 0;
            for(size_t i = 0; i < materials->size(); ++i){//for every material
                string texName = materials->at(i)->getTextureNames().at(0);//texture name
                if(loaded.count(texName) == 0){//if not loaded
                    textures->loadNew(c.cwd + tmp2 + "\\\\" + texName);//load by path
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
            c.materialLists.push_back(materials);
        }
    };

    class LightCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);
            string tmp1 = bite(" ", c.args);

            //x,y,z,name
            c.model->addNewLightSource(x, y, z, tmp1);
        }
    };
    class CamCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);
            float movementSpeed = biteFloat(" ", c.args);
            float sensitivity = biteFloat(" ", c.args);
            string tmp1 = bite(" ", c.args);

            //x,y,z,name
            c.model->addNewCamera(x, y, z, movementSpeed, sensitivity, tmp1);
        }
    };

    class FrmbCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            size_t itmp1 = biteInt(" ", c.args);
            size_t itmp2 = biteInt(" ", c.args);
            size_t itmp3 = biteInt(" ", c.args);
            string tmp1 = bite(" ", c.args);

            Framebuffer* framebuffer = new Framebuffer(itmp1, itmp2, tmp1);
            framebuffer->genTextureColorBuffers(itmp3);
            c.model->addFramebuffer(*framebuffer);

            MaterialList* frmbmat = new MaterialList;
            frmbmat->pushNew();

            frmbmat->name = tmp1;
            frmbmat->textures = framebuffer->textureColorBuffers;
            c.materialLists.push_back(frmbmat);
        }
    };

    template<class T>
    class MoveCommand : public ICommand<LoaderContext>{
    public:
        MoveCommand(){
            static_assert(std::is_base_of<IObject, T>::value, "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override{
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->move(x,y,z);
        }
        T* o;
    };

    template<class T>
    class ScaleCommand : public ICommand<LoaderContext>{
    public:
        ScaleCommand(){
            static_assert(std::is_base_of<IObject, T>::value, "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override{
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->scaleTo(x,y,z);
        }
        T* o;
    };

    template<class T>
    class RotCommand : public ICommand<LoaderContext>{
    public:
        RotCommand(){
            static_assert(std::is_base_of<IObject, T>::value, "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override{
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->rotateTo(x,y,z);
        }
        T* o;
    };

    class MeshObjCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp3 = bite(" ", c.args);
            string tmp4 = bite(" ", c.args);

            size_t which1 = 0, which2 = 0;

            for(size_t i = 0; i < c.materialLists.size(); ++i){
                if(c.materialLists.at(i)->name == tmp1){
                    which1 = i;
                    break;
                }
            }

            for(size_t i = 0; i < c.meshBuffers.size(); ++i){
                if(c.meshBuffers.at(i)->name == tmp2){
                    which2 = i;
                    break;
                }
            }
            //remove textures from addNewObject cause it moved it Materials
            c.model->addNewObject(*(c.meshBuffers).at(which2),
                                  c.materialLists.at(which1), tmp4);

            string nextCommand = c.getNextCommand();
            while(nextCommand == "move" || nextCommand == "scale" || nextCommand == "rot"){
                if(nextCommand == "move"){
                    c.step();
                    momovec.o = c.model->getMeshObject(tmp4);
                    momovec.execute(c);
                }
                if(nextCommand == "scale"){
                    c.step();
                    moscalec.o = c.model->getMeshObject(tmp4);
                    moscalec.execute(c);
                }
                if(nextCommand == "rot"){
                    c.step();
                    morotc.o = c.model->getMeshObject(tmp4);
                    morotc.execute(c);
                }
                nextCommand = c.getNextCommand();
            }
        }
    };

    class SklObjCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp4 = bite(" ", c.args);
            string tmp5 = c.cwd + bite(" ", c.args);
            float ftmp1 = biteFloat(" ", c.args);
            float ftmp2 = biteFloat(" ", c.args);

            size_t which1 = 0, which2 = 0;

            for(size_t i = 0; i < c.materialLists.size(); ++i){
                if(c.materialLists.at(i)->name == tmp1){
                    which1 = i;
                    break;
                }
            }


            for(size_t i = 0; i < c.sklBuffers.size(); ++i){
                if(c.sklBuffers.at(i)->name == tmp2){
                    which2 = i;
                    break;
                }
            }

            c.model->addNewObject(*c.sklBuffers.at(which2),
                                   c.materialLists.at(which1), tmp4);
            c.model->getSkeletonObject(tmp4)->parseAndPushAnimation(tmp5, ftmp1, ftmp2, tmp5);//add name instead of last arg
            c.model->getSkeletonObject(tmp4)->setCurrAnimation(tmp5);
            c.model->getSkeletonObject(tmp4)->startAnimation();

            string nextCommand = c.getNextCommand();
            while(nextCommand == "move" || nextCommand == "scale" || nextCommand == "rot"){
                if(nextCommand == "move"){
                    c.step();
                    somovec.o = c.model->getSkeletonObject(tmp4);
                    somovec.execute(c);
                }
                if(nextCommand == "scale"){
                    c.step();
                    soscalec.o = c.model->getSkeletonObject(tmp4);
                    soscalec.execute(c);
                }
                if(nextCommand == "rot"){
                    c.step();
                    sorotc.o = c.model->getSkeletonObject(tmp4);
                    sorotc.execute(c);
                }
            }
            nextCommand = c.getNextCommand();
        }
    };

    class InstObjCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp1 = bite(" ", c.args);
            string tmp2 = bite(" ", c.args);
            string tmp5 = bite(" ", c.args);

            size_t which1 = 0, which2 = 0;

            for(size_t i = 0; i < c.materialLists.size(); ++i){
                if(c.materialLists.at(i)->name == tmp1){
                    which1 = i;
                    break;
                }
            }

            for(size_t i = 0; i < c.instBuffers.size(); ++i){
                if(c.instBuffers.at(i)->name == tmp2){
                    which2 = i;
                    break;
                }
            }

            vector<Position> poses;

            c.model->addNewObject(*c.instBuffers.at(which2), c.materialLists.at(which1), poses, tmp5);

            string nextCommand = c.getNextCommand();
            if(nextCommand == "instel"){
                c.step();
                instelc.execute(c);
            }
        }
    };

    class InstElCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            InstancedObject* lastInstObj = c.model->getInstancedObject(c.model->instancedObjects->size()-1);
            lastInstObj->modelMatrices.push_back(Position());

            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            lastInstObj->moveTo(lastInstObj->modelMatrices.size() - 1, x, y, z);

            x = biteFloat(" ", c.args);
            y = biteFloat(" ", c.args);
            z = biteFloat(" ", c.args);

            lastInstObj->rotateTo(lastInstObj->modelMatrices.size() - 1, x, y, z);

            x = biteFloat(" ", c.args);
            y = biteFloat(" ", c.args);
            z = biteFloat(" ", c.args);

            lastInstObj->scaleTo(lastInstObj->modelMatrices.size() - 1, x, y, z);

            string nextCommand = c.getNextCommand();
            if(nextCommand == "instel"){
                c.step();
                instelc.execute(c);
            }
        }
    };

    class SkyboxCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            string tmp2 = bite(" ", c.args);

            vector<string> skyboxSides;
            skyboxSides.push_back(c.cwd + bite(" ", c.args));
            skyboxSides.push_back(c.cwd + bite(" ", c.args));
            skyboxSides.push_back(c.cwd + bite(" ", c.args));
            skyboxSides.push_back(c.cwd + bite(" ", c.args));
            skyboxSides.push_back(c.cwd + bite(" ", c.args));
            skyboxSides.push_back(c.cwd + bite(" ", c.args));

            string tmp4 = bite(" ", c.args);

            size_t which = 0;

            for(size_t i = 0; i < c.meshBuffers.size(); ++i){
                if(c.meshBuffers.at(i)->name == tmp2){
                    which = i;
                    break;
                }
            }

            c.model->addNewSkybox(skyboxSides, *c.meshBuffers.at(which), tmp4);

            string nextCommand = c.getNextCommand();
            while(nextCommand == "move" || nextCommand == "scale" || nextCommand == "rot"){
                if(nextCommand == "move"){
                    c.step();
                    skmovec.o = c.model->getSkyboxObject(tmp4);
                    skmovec.execute(c);
                }
                if(nextCommand == "scale"){
                    c.step();
                    skscalec.o = c.model->getSkyboxObject(tmp4);
                    skscalec.execute(c);
                }
                if(nextCommand == "rot"){
                    c.step();
                    skrotc.o = c.model->getSkyboxObject(tmp4);
                    skrotc.execute(c);
                }
                nextCommand = c.getNextCommand();
            }
        }
    };

    class BckColCommand : public ICommand<LoaderContext>{
    public:
        void execute(LoaderContext& c) override{
            float r, g, b, a;

            r = biteFloat(" ", c.args);
            g = biteFloat(" ", c.args);
            b = biteFloat(" ", c.args);
            a = biteFloat(" ", c.args);

            c.model->setBackgroundColor(r, g, b, a);
        }
    };

    static CwdCommand cwdc;
    static MeshCommand meshc;
    static SklCommand sklc;
    static ShadCommand shadc;
    static MeshBufCommand meshbufc;
    static SklBufCommand sklbufc;
    static InstBufCommand instbufc;
    static TexLCommand texlc;
    static TexMCommand texmc;
    static TexCommand texc;
    static MatCommand matc;
    static LightCommand lightc;
    static CamCommand camc;
    static FrmbCommand frmbc;
    static MoveCommand<MeshObject> momovec;
    static MoveCommand<SkeletonObject> somovec;
    static MoveCommand<SkyboxObject> skmovec;
    static ScaleCommand<MeshObject> moscalec;
    static ScaleCommand<SkeletonObject> soscalec;
    static ScaleCommand<SkyboxObject> skscalec;
    static RotCommand<MeshObject> morotc;
    static RotCommand<SkeletonObject> sorotc;
    static RotCommand<SkyboxObject> skrotc;
    static MeshObjCommand meshobjc;
    static SklObjCommand sklobjc;
    static InstObjCommand instobjc;
    static InstElCommand instelc;
    static SkyboxCommand skyboxc;
    static BckColCommand bckcolc;
};

SceneLoader::CwdCommand SceneLoader::cwdc;
SceneLoader::MeshCommand SceneLoader::meshc;
SceneLoader::SklCommand SceneLoader::sklc;
SceneLoader::ShadCommand SceneLoader::shadc;
SceneLoader::MeshBufCommand SceneLoader::meshbufc;
SceneLoader::SklBufCommand SceneLoader::sklbufc;
SceneLoader::InstBufCommand SceneLoader::instbufc;
SceneLoader::TexLCommand SceneLoader::texlc;
SceneLoader::TexMCommand SceneLoader::texmc;
SceneLoader::TexCommand SceneLoader::texc;
SceneLoader::MatCommand SceneLoader::matc;
SceneLoader::LightCommand SceneLoader::lightc;
SceneLoader::CamCommand SceneLoader::camc;
SceneLoader::FrmbCommand SceneLoader::frmbc;
SceneLoader::MoveCommand<MeshObject> SceneLoader::momovec;
SceneLoader::MoveCommand<SkeletonObject> SceneLoader::somovec;
SceneLoader::MoveCommand<SkyboxObject> SceneLoader::skmovec;
SceneLoader::ScaleCommand<MeshObject> SceneLoader::moscalec;
SceneLoader::ScaleCommand<SkeletonObject> SceneLoader::soscalec;
SceneLoader::ScaleCommand<SkyboxObject> SceneLoader::skscalec;
SceneLoader::RotCommand<MeshObject> SceneLoader::morotc;
SceneLoader::RotCommand<SkeletonObject> SceneLoader::sorotc;
SceneLoader::RotCommand<SkyboxObject> SceneLoader::skrotc;
SceneLoader::MeshObjCommand SceneLoader::meshobjc;
SceneLoader::SklObjCommand SceneLoader::sklobjc;
SceneLoader::InstObjCommand SceneLoader::instobjc;
SceneLoader::InstElCommand SceneLoader::instelc;
SceneLoader::SkyboxCommand SceneLoader::skyboxc;
SceneLoader::BckColCommand SceneLoader::bckcolc;
