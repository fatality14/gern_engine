#pragma once

#include <common/common.h>
#include <common/icontext.h>

template <class U, class R = void> class ICommand : public ICommon {
public:
    ICommand() {
        static_assert(std::is_base_of<IContext, U>::value,
                      "Template parameter U must be derived from IContext");
    }
    virtual R execute(U& context) = 0;
};