//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Object
//

#include "craftnapi/object.hpp"
#include "craftnapi/value.hpp"
#include "craftnapi/error.hpp"

namespace craftnapi {

Object::Object(napi_env env, napi_value value)
    : m_env(env)
    , m_value(value)
{}

MaybeError Object::set_named_property(const std::string& k, const Value& v) noexcept {
    if (auto status = napi_set_named_property(m_env, m_value, k.c_str(), v.to_napi()); status != napi_ok) {
        return make_error_code(status);
    }
    return craftpp::result::success();
}

Result<Value> Object::named_property(const std::string& k) const noexcept {
    napi_value result;
    if (auto status = napi_get_named_property(m_env, m_value, k.c_str(), &result); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, result);
}

Result<Maybe<Value>> Object::maybe_named_property(const std::string& k) const noexcept {
    bool has_property = false;
    if (auto status = napi_has_named_property(m_env, m_value, k.c_str(), &has_property); status != napi_ok) {
        return make_error_code(status);
    }
    if (!has_property) {
        return Maybe<Value>{craftpp::maybe::none()};
    }
    return named_property(k)
        .fmap([](Value&& val) {
            return Maybe<Value>{std::move(val)};
        });
}

}
