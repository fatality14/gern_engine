#pragma once

#include <common/common.h>
#include <render/iframemodel.h>

class IContext : public ICommon {};

template <class U, class R = void> class ICommand : public ICommon {
public:
    ICommand() {
        static_assert(std::is_base_of<IContext, U>::value,
                      "Template parameter U must be derived from IContext");
    }
    virtual R execute(U& context) = 0;
};

template <class T> class ISceneLoader : public ICommon {
public:
    ISceneLoader() {
        static_assert(std::is_base_of<IFrameModel, T>::value,
                      "Template parameter T must be derived from IFrameModel");
    }
    virtual void load(string path, T& data) = 0;
};
