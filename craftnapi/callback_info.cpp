//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Callback info
//

#include "craftnapi/callback_info.hpp"

namespace craftnapi {

CallbackInfo::CallbackInfo(Value self_arg, std::vector<Value>&& values)
    : this_arg(self_arg)
    , m_args(std::move(values))
{}

Result<Value> CallbackInfo::operator[](size_t index) const noexcept {
    if (index >= m_args.size()) {
        return make_error_code(WrapperError::NO_REQUIRED_ARGUMENT);
    }
    return m_args[index];
}

}
