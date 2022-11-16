#pragma once

#include <common/common.h>
#include <render/iframemodel.h>

template <class T> class ISceneLoader : public ICommon {
public:
    ISceneLoader() {
        static_assert(std::is_base_of<IFrameModel, T>::value,
                      "Template parameter T must be derived from IFrameModel");
    }
    virtual void load(const filesystem::path& path, T& data) = 0;
};
