//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Callback info
//

#pragma once

#include "craftnapi/common.hpp"
#include "craftnapi/value.hpp"

namespace craftnapi {

class CallbackInfo {
public:
    CallbackInfo(Value this_arg, std::vector<Value>&&);

    Value this_arg;

    Result<Value> operator[](size_t index) const noexcept;

private:
    std::vector<Value> m_args;
};

}
