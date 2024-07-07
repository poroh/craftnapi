//
// Copyright (c) 2023 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI C++ wrapper
//

#include "craftnapi/array.hpp"
#include "craftnapi/error.hpp"

namespace craftnapi {

Array::Array(napi_env env, napi_value v)
    : m_env(env)
    , m_value(v)
{}

MaybeError Array::set_element(size_t i, const Value& v) noexcept {
    if (auto status = napi_set_element(m_env, m_value, i, v.to_napi())) {
        return make_error_code(status);
    }
    return craftpp::result::success();
}

}
