//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Value
//

#include "craftnapi/value.hpp"
#include "craftnapi/error.hpp"
#include "craftnapi/object.hpp"

namespace craftnapi {

Value::Value(napi_env env, napi_value value)
    : m_env(env)
    , m_value(value)
{}

Result<ConstBinaryView> Value::as_buffer() const noexcept {
    bool is_buffer;
    if (auto status = napi_is_buffer(m_env, m_value, &is_buffer); status != napi_ok) {
        return make_error_code(status);
    }
    if (!is_buffer) {
        return make_error_code(WrapperError::invalid_type);
    }

    void *data;
    size_t length;
    if (auto status = napi_get_buffer_info(m_env, m_value, &data, &length); status != napi_ok) {
        return make_error_code(status);
    }

    return ConstBinaryView(data, length);
}

Result<Object> Value::as_object() const noexcept {
    napi_valuetype valuetype;
    if (auto status = napi_typeof(m_env, m_value, &valuetype); status != napi_ok) {
        return make_error_code(status);
    }
    if (valuetype != napi_object) {
        return make_error_code(WrapperError::invalid_type);
    }
    return Object(m_env, m_value);
}

Result<std::string> Value::as_string() const noexcept {
    // Get the length of the string in bytes
    size_t sz;
    if (auto status = napi_get_value_string_utf8(m_env, m_value, nullptr, 0, &sz); status != napi_ok) {
        return make_error_code(status);
    }

    std::string str;
    str.resize(sz);

    if (auto status = napi_get_value_string_utf8(m_env, m_value, &str[0], sz + 1, &sz); status != napi_ok) {
        return make_error_code(status);
    }

    return str;
}

Result<int32_t> Value::as_int32() const noexcept {
    int32_t result;
    if (auto status = napi_get_value_int32(m_env, m_value, &result); status != napi_ok) {
        return make_error_code(status);
    }
    return result;
}

Result<bool> Value::as_boolean() const noexcept {
    bool result;
    if (auto status = napi_get_value_bool(m_env, m_value, &result); status != napi_ok) {
        return make_error_code(status);
    }
    return result;
}

Result<Object> Value::to_object(const Value& val) noexcept {
    return val.as_object();
}

}
