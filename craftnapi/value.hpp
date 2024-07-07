//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Value
//
#pragma once

#include "craftnapi/common.hpp"
#include "craftnapi/error.hpp"

namespace craftnapi {

class Object;

class Value {
public:
    Value(napi_env, napi_value);

    Result<ConstBinaryView> as_buffer() const noexcept;
    Result<Object> as_object() const noexcept;
    Result<std::string> as_string() const noexcept;
    Result<int32_t> as_int32() const noexcept;
    Result<bool> as_boolean() const noexcept;

    napi_value to_napi() const noexcept;
    static napi_value fmap_to_napi(const Value&) noexcept;

    template<typename T>
    Result<std::reference_wrapper<T>> unwrap() const noexcept;

    static Result<ConstBinaryView> to_buffer(const Value& obj) noexcept;
    static Result<Object> to_object(const Value& obj) noexcept;
    static Result<std::string> to_string(const Value& obj) noexcept;
    static Result<int32_t> to_int32(const Value& obj) noexcept;

private:
    napi_env m_env;
    napi_value m_value;
};

//
// implementation
//
inline napi_value Value::to_napi() const noexcept {
    return m_value;
}

inline napi_value Value::fmap_to_napi(const Value& v) noexcept {
    return v.to_napi();
}

template<typename T>
Result<std::reference_wrapper<T>> Value::unwrap() const noexcept {
    T *t;
    if (auto status = napi_unwrap(m_env, m_value, reinterpret_cast<void **>(&t)); status != napi_ok) {
        return make_error_code(status);
    }
    return std::ref(*t);
}

inline Result<ConstBinaryView> Value::to_buffer(const Value& val) noexcept {
    return val.as_buffer();
}

inline Result<std::string> Value::to_string(const Value& val) noexcept {
    return val.as_string();
}

inline Result<int32_t> Value::to_int32(const Value& val) noexcept {
    return val.as_int32();
}

}
