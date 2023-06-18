#pragma once

#include "shader/shader.h"
#include "shader/shaderloader.h"
#include <animation/poseloader.h>
#include <common/commanditerator.h>
#include <common/common.h>
#include <common/icommand.h>
#include <common/icontext.h>
#include <filesystem>
#include <framemodel.h>
#include <mesh/meshloader.h>
#include <mesh/skeletonloader.h>
#include <object/instancedobject.h>
#include <object/skeletonobject.h>
#include <render/isceneloader.h>
#include <texture/materialloader.h>

#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class SceneLoaderContext : public IContext {
public:
    SceneLoaderContext(
        const filesystem::path& filePath)
        : it{filePath, *this} {}

    CommandIterator<SceneLoaderContext> it;

    FrameModel* m;

    filesystem::path cwd;

    MeshLoader meshLoader;
    MaterialLoader materialLoader;
    PoseLoader poseLoader;
    ShaderLoader shaderLoader;
    SkeletonLoader skeletizer;
};

typedef ICommand<SceneLoaderContext> Command;

// TODO provide throw wherever it needed
class SceneLoader : private ULoader, public ISceneLoader<FrameModel> {
public:
    using cmdPair = pair<string, ICommand<SceneLoaderContext>&>;

    inline static map<string, ICommand<SceneLoaderContext>&> commandList;

    SceneLoader() {
        commandList = {cmdPair("cwd", cwdc),         cmdPair("mesh", meshc),
                    cmdPair("skl", sklc),         cmdPair("shad", shadc),
                    cmdPair("meshbuf", meshbufc), cmdPair("sklbuf", sklbufc),
                    cmdPair("instbuf", instbufc), cmdPair("tex", texc),
                    cmdPair("mat", matc),         cmdPair("light", lightc),
                    cmdPair("cam", camc),         cmdPair("frmb", frmbc),
                    cmdPair("meshobj", meshobjc), cmdPair("sklobj", sklobjc),
                    cmdPair("anm", anmc),         cmdPair("instobj", instobjc),
                    cmdPair("skybox", skyboxc),   cmdPair("bckcol", bckcolc)};
    }

    void load(const filesystem::path& path, FrameModel& model) override {
        c = new SceneLoaderContext(path);

        c->m = &model;

        while (c->it.step()) {
            c->it.execute(commandList);
        }
    }
    
    SceneLoaderContext* c;

private:
    class CwdCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            c.cwd = bite(" ", c.it.args);
        }
    };

    class MeshCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            filesystem::path path = c.cwd / bite(" ", c.it.args);
            string meshName = bite(" ", c.it.args);
            c.m->meshList.push(*c.meshLoader.load(path, meshName));
        }
    };

    class SklCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string meshName = bite(" ", c.it.args);
            filesystem::path skeletonPath = c.cwd / bite(" ", c.it.args);
            c.m->skeletonList.push(*c.skeletizer.skeletize(
                *(c.m->meshList).getByName(meshName), skeletonPath));
        }
    };

    class ShadCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string shaderName = bite(" ", c.it.args);

            Shader* shader = new Shader();
            shader->name = shaderName;

            if (c.it.isNextCommand("shadel")) {
                c.it.step();
                shadelc.shader = shader;
                shadelc.execute(c);
            } else {
                throw string("there must be a shadel command after shad "
                             "block in scene");
            }
        }
    };

    class ShadElCommand : public ICommand<SceneLoaderContext> {
    public:
        ShadElCommand() { paths = new vector<filesystem::path>(); }
        ~ShadElCommand() { delete paths; }

        void execute(SceneLoaderContext& c) override {
            filesystem::path path = c.cwd / bite(" ", c.it.args);
            paths->push_back(path);

            if (c.it.isNextCommand("shadel")) {
                c.it.step();
                // no need to pass paths to next execution while shadelc is
                // static, but it might not in future
                shadelc.execute(c);
            } else {
                shader->program = c.shaderLoader.load(*paths);
                paths->clear();
                c.m->shaders.push(*shader);
            }
        }

        vector<filesystem::path>* paths;
        Shader* shader;
    };

    class MeshBufCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string meshName = bite(" ", c.it.args);
            string shaderName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            c.m->meshBuffers.push_back(
                new MeshBuffer(*(c.m->meshList).getByName(meshName),
                               *(c.m->shaders).getByName(shaderName), bufferName));
            c.m->meshBuffers.at(c.m->meshBuffers.size() - 1)->genBuffers();
        }
    };

    class SklBufCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string meshName = bite(" ", c.it.args);
            string shaderName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            c.m->sklBuffers.push_back(new SkeletonBuffer(
                *(c.m->skeletonList).getByName(meshName),
                *(c.m->shaders).getByName(shaderName), bufferName));
            c.m->sklBuffers.at(c.m->sklBuffers.size() - 1)->genBuffers();
        }
    };

    class InstBufCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string meshName = bite(" ", c.it.args);
            string shaderName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            c.m->instBuffers.push_back(new InstancedBuffer(
                *(c.m->meshList).getByName(meshName),
                *(c.m->shaders).getByName(shaderName), bufferName));
            c.m->instBuffers.at(c.m->instBuffers.size() - 1)->genBuffers();
        }
    };

    class TexLCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            c.m->materialLists.at(c.m->materialLists.size() - 1)
                ->textures->addLayouts(1);

            size_t layoutId = biteInt(" ", c.it.args);
            GLuint textureUnit = biteInt(" ", c.it.args);
            size_t textureIndex = biteInt(" ", c.it.args);
            string uniformName = bite(" ", c.it.args);

            c.m->materialLists.at(c.m->materialLists.size() - 1)
                ->textures->appendTextureToLayout(layoutId, textureUnit,
                                                  textureIndex, uniformName);

            if (c.it.isNextCommand("texl")) {
                c.it.step();
                texlc.execute(c);
            }
        }
    };

    class TexMCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            c.m->materialLists.at(c.m->materialLists.size() - 1)
                ->textures->loadNew(c.cwd / c.it.args);

            if (c.it.isNextCommand("texm")) {
                c.it.step();
                texmc.execute(c);
            } else if (c.it.isNextCommand("texl")) {
                c.it.step();
                texlc.execute(c);
            } else {
                throw string("there must be a texl command after texm "
                             "block in scene");
            }
        }
    };

    class TexCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            MaterialList* texbmat = new MaterialList;
            texbmat->pushNew();
            texbmat->name = c.it.args;

            c.m->materialLists.push_back(texbmat);

            if (c.it.isNextCommand("texm")) {
                c.it.step();
                texmc.execute(c);
            } else {
                throw string("there must be a texm command after tex in scene");
            }
        }
    };

    class MatCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            // TODO throw error if there is no texture specified in .mat
            // else vector is out of borders

            // TODO fix to use other textures than map_Kd

            filesystem::path mtlPath = c.cwd / bite(" ", c.it.args);
            filesystem::path textureFolderPath = bite(" ", c.it.args);
            string materialListName = bite(" ", c.it.args);

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
            c.m->materialLists.push_back(materials);
        }
    };

    class LightCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);
            string name = bite(" ", c.it.args);

            c.m->addNewLightSource(x, y, z, name);
        }
    };
    class CamCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);
            float movementSpeed = biteFloat(" ", c.it.args);
            float sensitivity = biteFloat(" ", c.it.args);
            string name = bite(" ", c.it.args);

            c.m->addNewCamera(x, y, z, movementSpeed, sensitivity, name);
        }
    };

    class FrmbCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            size_t width = biteInt(" ", c.it.args);
            size_t height = biteInt(" ", c.it.args);
            size_t frmbAmount = biteInt(" ", c.it.args);
            string frmbName = bite(" ", c.it.args);

            Framebuffer* framebuffer = new Framebuffer(width, height, frmbName);
            framebuffer->appendTextureColorBuffers(frmbAmount);
            c.m->addFramebuffer(*framebuffer);

            MaterialList* frmbmat = new MaterialList;
            frmbmat->pushNew();

            frmbmat->name = frmbName;
            frmbmat->textures = framebuffer->textureColorBuffers;
            c.m->materialLists.push_back(frmbmat);
        }
    };

    template <class T> class MoveCommand : public ICommand<SceneLoaderContext> {
    public:
        MoveCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(SceneLoaderContext& c) override {
            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);

            o->move(x, y, z);
        }
        T* o;
    };

    template <class T>
    class ScaleCommand : public ICommand<SceneLoaderContext> {
    public:
        ScaleCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(SceneLoaderContext& c) override {
            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);

            o->scaleTo(x, y, z);
        }
        T* o;
    };

    template <class T> class RotCommand : public ICommand<SceneLoaderContext> {
    public:
        RotCommand() {
            static_assert(std::is_base_of<IObject, T>::value,
                          "Template parameter T must be derived from IObject");
        }

        void execute(SceneLoaderContext& c) override {
            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);

            o->rotateTo(x, y, z);
        }
        T* o;
    };

    class MeshObjCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string materialName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            string objName = bite(" ", c.it.args);

            size_t whichMtl = 0, whichMeshbuf = 0;

            for (size_t i = 0; i < c.m->materialLists.size(); ++i) {
                if (c.m->materialLists.at(i)->name == materialName) {
                    whichMtl = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.m->meshBuffers.size(); ++i) {
                if (c.m->meshBuffers.at(i)->name == bufferName) {
                    whichMeshbuf = i;
                    break;
                }
            }
            c.m->addNewObject(*(c.m->meshBuffers).at(whichMeshbuf),
                                  *c.m->materialLists.at(whichMtl), objName);

            string nextCommand = c.it.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.it.step();
                    momovec.o = &c.m->getMeshObject(objName);
                    momovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.it.step();
                    moscalec.o = &c.m->getMeshObject(objName);
                    moscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.it.step();
                    morotc.o = &c.m->getMeshObject(objName);
                    morotc.execute(c);
                }
                nextCommand = c.it.getNextCommand();
            }
        }
    };

    class SklObjCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string materialName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            string objName = bite(" ", c.it.args);

            size_t which1 = 0, which2 = 0;

            for (size_t i = 0; i < c.m->materialLists.size(); ++i) {
                if (c.m->materialLists.at(i)->name == materialName) {
                    which1 = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.m->sklBuffers.size(); ++i) {
                if (c.m->sklBuffers.at(i)->name == bufferName) {
                    which2 = i;
                    break;
                }
            }

            c.m->addNewObject(*c.m->sklBuffers.at(which2),
                                  c.m->materialLists.at(which1), objName);

            SkeletonObject* currObject = &c.m->getSkeletonObject(objName);

            string nextCommand = c.it.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.it.step();
                    somovec.o = currObject;
                    somovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.it.step();
                    soscalec.o = currObject;
                    soscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.it.step();
                    sorotc.o = currObject;
                    sorotc.execute(c);
                }
                nextCommand = c.it.getNextCommand();
            }

            if (c.it.isNextCommand("anm")) {
                c.it.step();
                anmc.o = currObject;
                anmc.execute(c);
            }
            currObject->startAnimation();
        }
    };

    class AnmCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            filesystem::path anmPath = c.cwd / bite(" ", c.it.args);
            float anmMult = biteFloat(" ", c.it.args);
            float anmStart = biteFloat(" ", c.it.args);

            PoseList* poses = c.poseLoader.parseKeyPoses(anmPath);
            Animation* anm = new Animation(o->buffer->getMesh().joints, anmMult,
                                           anmStart, anmPath.string());
            anm->setKeyPoses(*poses);

            o->addAnimation(*anm);
            o->setCurrAnimation(anmPath.string());

            string nextCommand = c.it.getNextCommand();
            if (c.it.isNextCommand("anm")) {
                c.it.step();
                // anmc.o = this->o; //no need to while anmc is static, but it
                // might not in future
                anmc.execute(c);
            }
        }

        SkeletonObject* o;
    };

    class InstObjCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string materialName = bite(" ", c.it.args);
            string bufferName = bite(" ", c.it.args);
            string objName = bite(" ", c.it.args);

            size_t which1 = 0, which2 = 0;

            for (size_t i = 0; i < c.m->materialLists.size(); ++i) {
                if (c.m->materialLists.at(i)->name == materialName) {
                    which1 = i;
                    break;
                }
            }

            for (size_t i = 0; i < c.m->instBuffers.size(); ++i) {
                if (c.m->instBuffers.at(i)->name == bufferName) {
                    which2 = i;
                    break;
                }
            }

            vector<Position> poses;

            c.m->addNewObject(*c.m->instBuffers.at(which2),
                                  c.m->materialLists.at(which1), poses, objName);

            if (c.it.isNextCommand("instel")) {
                c.it.step();
                instelc.execute(c);
            }
        }
    };

    class InstElCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            InstancedObject* lastInstObj = &c.m->getInstancedObject(
                c.m->instancedObjects->size() - 1);
            lastInstObj->modelMatrices.push_back(Position());

            float x = biteFloat(" ", c.it.args);
            float y = biteFloat(" ", c.it.args);
            float z = biteFloat(" ", c.it.args);

            lastInstObj->moveTo(lastInstObj->modelMatrices.size() - 1, x, y, z);

            x = biteFloat(" ", c.it.args);
            y = biteFloat(" ", c.it.args);
            z = biteFloat(" ", c.it.args);

            lastInstObj->rotateTo(lastInstObj->modelMatrices.size() - 1, x, y,
                                  z);

            x = biteFloat(" ", c.it.args);
            y = biteFloat(" ", c.it.args);
            z = biteFloat(" ", c.it.args);

            lastInstObj->scaleTo(lastInstObj->modelMatrices.size() - 1, x, y,
                                 z);

            if (c.it.isNextCommand("instel")) {
                c.it.step();
                instelc.execute(c);
            }
        }
    };

    class SkyboxCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            string bufferName = bite(" ", c.it.args);

            array<filesystem::path, 6> skyboxSides;
            for_each(skyboxSides.begin(), skyboxSides.end(),
                     [&c](auto& i) { i = c.cwd / bite(" ", c.it.args); });

            string tmp4 = bite(" ", c.it.args);

            size_t which = 0;

            for (size_t i = 0; i < c.m->meshBuffers.size(); ++i) {
                if (c.m->meshBuffers.at(i)->name == bufferName) {
                    which = i;
                    break;
                }
            }

            c.m->addNewSkybox(skyboxSides, *c.m->meshBuffers.at(which), tmp4);

            string nextCommand = c.it.getNextCommand();
            while (nextCommand == "move" || nextCommand == "scale" ||
                   nextCommand == "rot") {
                if (nextCommand == "move") {
                    c.it.step();
                    skmovec.o = &c.m->getSkyboxObject(tmp4);
                    skmovec.execute(c);
                }
                if (nextCommand == "scale") {
                    c.it.step();
                    skscalec.o = &c.m->getSkyboxObject(tmp4);
                    skscalec.execute(c);
                }
                if (nextCommand == "rot") {
                    c.it.step();
                    skrotc.o = &c.m->getSkyboxObject(tmp4);
                    skrotc.execute(c);
                }
                nextCommand = c.it.getNextCommand();
            }
        }
    };

    class BckColCommand : public ICommand<SceneLoaderContext> {
    public:
        void execute(SceneLoaderContext& c) override {
            float r, g, b, a;

            r = biteFloat(" ", c.it.args);
            g = biteFloat(" ", c.it.args);
            b = biteFloat(" ", c.it.args);
            a = biteFloat(" ", c.it.args);

            c.m->setBackgroundColor(r, g, b, a);
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
