#pragma once

#include "shader/shader.h"
#include "shader/shaderloader.h"
#include <animation/poseloader.h>
#include <common/common.h>
#include <filesystem>
#include <framemodel.h>
#include <mesh/meshloader.h>
#include <mesh/skeletonloader.h>
#include <object/instancedobject.h>
#include <object/skeletonobject.h>
#include <render/isceneloader.h>
#include <string>
#include <texture/materialloader.h>

#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template<typename T>
class FileParser : public ICommon{
    bool step() {
        if (lines.size() != 0) {
            string& currLine = lines.front();

            ULoader::removeExcessSpaces(currLine);

            command = ULoader::bite(" ", currLine);
            args = currLine;

            lines.pop();

            return true;
        } else {
            return false;
        }
    }

    string getNextCommand() {
        string nextCommand = lines.front();
        return ULoader::bite(" ", nextCommand);
    }

    bool isNextCommand(const string& command) {
        return getNextCommand() == command;
    }

    void execute(const map<string, ICommand<T>&>& commands) {
        bool found = commands.count(command);
        if(found){
            commands.at(command).execute(*this);
        }
        else{
            //TODO cout path or name of file
            cout << "Warning: no such command " << command << " found while loading scene" << endl;
        }
    }

    queue<string> lines;
    string args;
    string command;
};

class LoaderContext : public IContext {
public:
    ~LoaderContext() {
        for (size_t i = 0; i < meshBuffers.size(); ++i) {
            delete meshBuffers[i];
        }
        for (size_t i = 0; i < sklBuffers.size(); ++i) {
            delete sklBuffers[i];
        }
        for (size_t i = 0; i < instBuffers.size(); ++i) {
            delete instBuffers[i];
        }
        for (size_t i = 0; i < materialLists.size(); ++i) {
            delete materialLists[i];
        }
    }

    bool step() {
        if (lines.size() != 0) {
            string& currLine = lines.front();

            ULoader::removeExcessSpaces(currLine);

            command = ULoader::bite(" ", currLine);
            args = currLine;

            lines.pop();

            return true;
        } else {
            return false;
        }
    }

    string getNextCommand() {
        string nextCommand = lines.front();
        return ULoader::bite(" ", nextCommand);
    }

    bool isNextCommand(const string& command) {
        return getNextCommand() == command;
    }

    void execute(const map<string, ICommand<LoaderContext>&>& commands) {
        bool found = commands.count(command);
        if(found){
            commands.at(command).execute(*this);
        }
        else{
            //TODO cout path or name of file
            cout << "Warning: no such command " << command << " found while loading scene" << endl;
        }
    }

    filesystem::path cwd;

    queue<string> lines;
    string args;
    string command;
    
    FrameModel* model;
    MeshLoader meshLoader;
    MeshList meshList;
    SkeletonMeshList skeletonList;
    SkeletonLoader skeletizer;
    MaterialLoader materialLoader;
    ShaderList shaders;
    vector<MaterialList*> materialLists;

    PoseLoader poseLoader;
    ShaderLoader shaderLoader;

    vector<MeshBuffer*> meshBuffers;
    vector<SkeletonBuffer*> sklBuffers;
    vector<InstancedBuffer*> instBuffers;
};

typedef ICommand<LoaderContext> Command;

// TODO provide throw wherever it needed
class SceneLoader : private ULoader, public ISceneLoader<FrameModel> {
public:
    using cmdPair = pair<string, ICommand<LoaderContext>&>;

    inline static map<string, ICommand<LoaderContext>&> mainCmds;

    SceneLoader() {
        mainCmds = {cmdPair("cwd", cwdc),         cmdPair("mesh", meshc),
                    cmdPair("skl", sklc),         cmdPair("shad", shadc),
                    cmdPair("meshbuf", meshbufc), cmdPair("sklbuf", sklbufc),
                    cmdPair("instbuf", instbufc), cmdPair("tex", texc),
                    cmdPair("mat", matc),         cmdPair("light", lightc),
                    cmdPair("cam", camc),         cmdPair("frmb", frmbc),
                    cmdPair("meshobj", meshobjc), cmdPair("sklobj", sklobjc),
                    cmdPair("anm", anmc),         cmdPair("instobj", instobjc),
                    cmdPair("skybox", skyboxc),   cmdPair("bckcol", bckcolc)};
    }

    void load(const filesystem::path& path, FrameModel& data) override {
        c.model = &data;

        ifstream f;
        f.open(path);

        string line;

        if (f.fail()) {
            throw string("Cannot open file: ") + path.string();
        }

        while (!f.eof()) {
            getline(f, line);
            removeExcessSpaces(line);
            c.lines.push(line);
        }

        while (c.step()) {
            c.execute(mainCmds);
        }
    }

    LoaderContext c;

private:
    class CwdCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override { c.cwd = bite(" ", c.args); }
    };

    class MeshCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            filesystem::path path = c.cwd / bite(" ", c.args);
            string meshName = bite(" ", c.args);
            c.meshList.push(c.meshLoader.load(path, meshName));
        }
    };

    class SklCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string meshName = bite(" ", c.args);
            filesystem::path skeletonPath = c.cwd / bite(" ", c.args);
            c.skeletonList.push(c.skeletizer.skeletize(
                *(c.meshList).getByName(meshName), skeletonPath));
        }
    };

    class ShadCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string shaderName = bite(" ", c.args);

            Shader* shader = new Shader();
            shader->name = shaderName;

            if (c.isNextCommand("shadel")) {
                c.step();
                shadelc.shader = shader;
                shadelc.execute(c);
            } else {
                throw string("there must be a shadel command after shad "
                             "block in scene");
            }
        }
    };

    class ShadElCommand : public ICommand<LoaderContext> {
    public:
        ShadElCommand() { paths = new vector<filesystem::path>(); }
        ~ShadElCommand() { delete paths; }

        void execute(LoaderContext& c) override {
            filesystem::path path = c.cwd / bite(" ", c.args);
            paths->push_back(path);

            if (c.isNextCommand("shadel")) {
                c.step();
                // no need to pass paths to next execution while shadelc is
                // static, but it might not in future
                shadelc.execute(c);
            } else {
                shader->program = c.shaderLoader.load(*paths);
                paths->clear();
                c.shaders.push(*shader);
            }
        }

        vector<filesystem::path>* paths;
        Shader* shader;
    };

    class MeshBufCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string meshName = bite(" ", c.args);
            string shaderName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            c.meshBuffers.push_back(
                new MeshBuffer(*(c.meshList).getByName(meshName),
                               *(c.shaders).getByName(shaderName), bufferName));
            c.meshBuffers.at(c.meshBuffers.size() - 1)->genBuffers();
        }
    };

    class SklBufCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string meshName = bite(" ", c.args);
            string shaderName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            c.sklBuffers.push_back(new SkeletonBuffer(
                *(c.skeletonList).getByName(meshName),
                *(c.shaders).getByName(shaderName), bufferName));
            c.sklBuffers.at(c.sklBuffers.size() - 1)->genBuffers();
        }
    };

    class InstBufCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string meshName = bite(" ", c.args);
            string shaderName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            c.instBuffers.push_back(new InstancedBuffer(
                *(c.meshList).getByName(meshName),
                *(c.shaders).getByName(shaderName), bufferName));
            c.instBuffers.at(c.instBuffers.size() - 1)->genBuffers();
        }
    };

    class TexLCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            c.materialLists.at(c.materialLists.size() - 1)
                ->textures->addLayouts(1);

            size_t layoutId = biteInt(" ", c.args);
            GLuint textureUnit = biteInt(" ", c.args);
            size_t textureIndex = biteInt(" ", c.args);
            string uniformName = bite(" ", c.args);

            c.materialLists.at(c.materialLists.size() - 1)
                ->textures->appendTextureToLayout(layoutId, textureUnit,
                                                  textureIndex, uniformName);

            if (c.isNextCommand("texl")) {
                c.step();
                texlc.execute(c);
            }
        }
    };

    class TexMCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            c.materialLists.at(c.materialLists.size() - 1)
                ->textures->loadNew(c.cwd / c.args);

            if (c.isNextCommand("texm")) {
                c.step();
                texmc.execute(c);
            } else if (c.isNextCommand("texl")) {
                c.step();
                texlc.execute(c);
            } else {
                throw string("there must be a texl command after texm "
                             "block in scene");
            }
        }
    };

    class TexCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            MaterialList* texbmat = new MaterialList;
            texbmat->pushNew();
            texbmat->name = c.args;

            c.materialLists.push_back(texbmat);

            if (c.isNextCommand("texm")) {
                c.step();
                texmc.execute(c);
            } else {
                throw string("there must be a texm command after tex in scene");
            }
        }
    };

    class MatCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            // TODO throw error if there is no texture specified in .mat
            // else vector is out of borders

            // TODO fix to use other textures than map_Kd

            filesystem::path mtlPath = c.cwd / bite(" ", c.args);
            filesystem::path textureFolderPath = bite(" ", c.args);
            string materialListName = bite(" ", c.args);

            MaterialList* materials = c.materialLoader.load(mtlPath);
            TextureList* textures = materials->textures;

            textures->addLayouts(
                materials->size()); // gen layout for each material

            unordered_set<string> loaded;
            unordered_map<string, size_t> ids;
            size_t idCounter = 0;
            for (size_t i = 0; i < materials->size();
                 ++i) { // for every material
                string texName =
                    materials->at(i)->getTextureNames().at(0); // texture name
                if (loaded.count(texName) == 0) {              // if not loaded
                    textures->loadNew(c.cwd / textureFolderPath /
                                      texName); // load by path
                    loaded.emplace(texName);    // mark loaded
                    ids.emplace(
                        pair(texName,
                             idCounter)); // map name to id in TextureList
                    ++idCounter;
                }
            }

            for (size_t i = 0; i < materials->size(); ++i) {
                string texName =
                    materials->at(i)->getTextureNames().at(0); // texture name
                textures->appendTextureToLayout(i, 0, ids.at(texName),
                                                "texture0");
            }

            materials->name = materialListName;
            c.materialLists.push_back(materials);
        }
    };

    class LightCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);
            string name = bite(" ", c.args);

            c.model->addNewLightSource(x, y, z, name);
        }
    };
    class CamCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);
            float movementSpeed = biteFloat(" ", c.args);
            float sensitivity = biteFloat(" ", c.args);
            string name = bite(" ", c.args);

            c.model->addNewCamera(x, y, z, movementSpeed, sensitivity, name);
        }
    };

    class FrmbCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            size_t width = biteInt(" ", c.args);
            size_t height = biteInt(" ", c.args);
            size_t frmbAmount = biteInt(" ", c.args);
            string frmbName = bite(" ", c.args);

            Framebuffer* framebuffer = new Framebuffer(width, height, frmbName);
            framebuffer->appendTextureColorBuffers(frmbAmount);
            c.model->addFramebuffer(*framebuffer);

            MaterialList* frmbmat = new MaterialList;
            frmbmat->pushNew();

            frmbmat->name = frmbName;
            frmbmat->textures = framebuffer->textureColorBuffers;
            c.materialLists.push_back(frmbmat);
        }
    };

    template <class T> class MoveCommand : public ICommand<LoaderContext> {
    public:
        MoveCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override {
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->move(x, y, z);
        }
        T* o;
    };

    template <class T> class ScaleCommand : public ICommand<LoaderContext> {
    public:
        ScaleCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override {
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->scaleTo(x, y, z);
        }
        T* o;
    };

    template <class T> class RotCommand : public ICommand<LoaderContext> {
    public:
        RotCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(LoaderContext& c) override {
            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            o->rotateTo(x, y, z);
        }
        T* o;
    };

    class MeshObjCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string materialName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            string objName = bite(" ", c.args);

            size_t whichMtl = 0, whichMeshbuf = 0;

            for (size_t i = 0; i < c.materialLists.size(); ++i) {
                if (c.materialLists.at(i)->name == materialName) {
                    whichMtl = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.meshBuffers.size(); ++i) {
                if (c.meshBuffers.at(i)->name == bufferName) {
                    whichMeshbuf = i;
                    break;
                }
            }
            c.model->addNewObject(*(c.meshBuffers).at(whichMeshbuf),
                                  *c.materialLists.at(whichMtl), objName);

            string nextCommand = c.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.step();
                    momovec.o = &c.model->getMeshObject(objName);
                    momovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.step();
                    moscalec.o = &c.model->getMeshObject(objName);
                    moscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.step();
                    morotc.o = &c.model->getMeshObject(objName);
                    morotc.execute(c);
                }
                nextCommand = c.getNextCommand();
            }
        }
    };

    class SklObjCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string materialName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            string objName = bite(" ", c.args);

            size_t which1 = 0, which2 = 0;

            for (size_t i = 0; i < c.materialLists.size(); ++i) {
                if (c.materialLists.at(i)->name == materialName) {
                    which1 = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.sklBuffers.size(); ++i) {
                if (c.sklBuffers.at(i)->name == bufferName) {
                    which2 = i;
                    break;
                }
            }

            c.model->addNewObject(*c.sklBuffers.at(which2),
                                  c.materialLists.at(which1), objName);

            SkeletonObject* currObject = &c.model->getSkeletonObject(objName);

            string nextCommand = c.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.step();
                    somovec.o = currObject;
                    somovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.step();
                    soscalec.o = currObject;
                    soscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.step();
                    sorotc.o = currObject;
                    sorotc.execute(c);
                }
                nextCommand = c.getNextCommand();
            }

            if (c.isNextCommand("anm")) {
                c.step();
                anmc.o = currObject;
                anmc.execute(c);
            }
            currObject->startAnimation();
        }
    };

    class AnmCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            filesystem::path anmPath = c.cwd / bite(" ", c.args);
            float anmMult = biteFloat(" ", c.args);
            float anmStart = biteFloat(" ", c.args);

            PoseList* poses = c.poseLoader.parseKeyPoses(anmPath);
            Animation* anm = new Animation(o->buffer->getMesh().joints, anmMult,
                                           anmStart, anmPath);
            anm->setKeyPoses(*poses);

            o->addAnimation(*anm);
            o->setCurrAnimation(anmPath);

            string nextCommand = c.getNextCommand();
            if (c.isNextCommand("anm")) {
                c.step();
                // anmc.o = this->o; //no need to while anmc is static, but it
                // might not in future
                anmc.execute(c);
            }
        }

        SkeletonObject* o;
    };

    class InstObjCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string materialName = bite(" ", c.args);
            string bufferName = bite(" ", c.args);
            string objName = bite(" ", c.args);

            size_t which1 = 0, which2 = 0;

            for (size_t i = 0; i < c.materialLists.size(); ++i) {
                if (c.materialLists.at(i)->name == materialName) {
                    which1 = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.instBuffers.size(); ++i) {
                if (c.instBuffers.at(i)->name == bufferName) {
                    which2 = i;
                    break;
                }
            }

            vector<Position> poses;

            c.model->addNewObject(*c.instBuffers.at(which2),
                                  c.materialLists.at(which1), poses, objName);

            if (c.isNextCommand("instel")) {
                c.step();
                instelc.execute(c);
            }
        }
    };

    class InstElCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            InstancedObject* lastInstObj = &c.model->getInstancedObject(
                c.model->instancedObjects->size() - 1);
            lastInstObj->modelMatrices.push_back(Position());

            float x = biteFloat(" ", c.args);
            float y = biteFloat(" ", c.args);
            float z = biteFloat(" ", c.args);

            lastInstObj->moveTo(lastInstObj->modelMatrices.size() - 1, x, y, z);

            x = biteFloat(" ", c.args);
            y = biteFloat(" ", c.args);
            z = biteFloat(" ", c.args);

            lastInstObj->rotateTo(lastInstObj->modelMatrices.size() - 1, x, y,
                                  z);

            x = biteFloat(" ", c.args);
            y = biteFloat(" ", c.args);
            z = biteFloat(" ", c.args);

            lastInstObj->scaleTo(lastInstObj->modelMatrices.size() - 1, x, y,
                                 z);

            if (c.isNextCommand("instel")) {
                c.step();
                instelc.execute(c);
            }
        }
    };

    class SkyboxCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            string bufferName = bite(" ", c.args);

            array<filesystem::path, 6> skyboxSides;
            for_each(skyboxSides.begin(), skyboxSides.end(),
                     [&c](auto& i) { i = c.cwd / bite(" ", c.args); });

            string tmp4 = bite(" ", c.args);

            size_t which = 0;

            for (size_t i = 0; i < c.meshBuffers.size(); ++i) {
                if (c.meshBuffers.at(i)->name == bufferName) {
                    which = i;
                    break;
                }
            }

            c.model->addNewSkybox(skyboxSides, *c.meshBuffers.at(which), tmp4);

            string nextCommand = c.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.step();
                    skmovec.o = &c.model->getSkyboxObject(tmp4);
                    skmovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.step();
                    skscalec.o = &c.model->getSkyboxObject(tmp4);
                    skscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.step();
                    skrotc.o = &c.model->getSkyboxObject(tmp4);
                    skrotc.execute(c);
                }
                nextCommand = c.getNextCommand();
            }
        }
    };

    class BckColCommand : public ICommand<LoaderContext> {
    public:
        void execute(LoaderContext& c) override {
            float r, g, b, a;

            r = biteFloat(" ", c.args);
            g = biteFloat(" ", c.args);
            b = biteFloat(" ", c.args);
            a = biteFloat(" ", c.args);

            c.model->setBackgroundColor(r, g, b, a);
        }
    };

    inline static CwdCommand cwdc;
    inline static MeshCommand meshc;
    inline static SklCommand sklc;
    inline static ShadCommand shadc;
    inline static ShadElCommand shadelc;
    inline static MeshBufCommand meshbufc;
    inline static SklBufCommand sklbufc;
    inline static InstBufCommand instbufc;
    inline static TexLCommand texlc;
    inline static TexMCommand texmc;
    inline static TexCommand texc;
    inline static MatCommand matc;
    inline static LightCommand lightc;
    inline static CamCommand camc;
    inline static FrmbCommand frmbc;
    inline static MoveCommand<MeshObject> momovec;
    inline static MoveCommand<SkeletonObject> somovec;
    inline static MoveCommand<SkyboxObject> skmovec;
    inline static ScaleCommand<MeshObject> moscalec;
    inline static ScaleCommand<SkeletonObject> soscalec;
    inline static ScaleCommand<SkyboxObject> skscalec;
    inline static RotCommand<MeshObject> morotc;
    inline static RotCommand<SkeletonObject> sorotc;
    inline static RotCommand<SkyboxObject> skrotc;
    inline static MeshObjCommand meshobjc;
    inline static SklObjCommand sklobjc;
    inline static AnmCommand anmc;
    inline static InstObjCommand instobjc;
    inline static InstElCommand instelc;
    inline static SkyboxCommand skyboxc;
    inline static BckColCommand bckcolc;
};