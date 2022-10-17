#pragma once

#include <common/common.h>
#include <render/iframemodel.h>

template <class T> class IFrameController : public ICommon {
public:
    IFrameController() {
        static_assert(std::is_base_of<IFrameModel, T>::value,
                      "Template parameter T must be derived from IFrameModel");
    }
    void frameFunction(T& data);
};
