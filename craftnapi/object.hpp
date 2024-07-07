//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Object Wrapper
//
#pragma once

#include "craftnapi/common.hpp"
#include "craftnapi/value.hpp"


namespace craftnapi {

class Object {
public:

    Object(napi_env, napi_value);

    MaybeError set_named_property(const std::string&, const Value&) noexcept;
    Value to_value() const noexcept;
    Result<Value> named_property(const std::string& name) const noexcept;
    Result<Maybe<Value>> maybe_named_property(const std::string& name) const noexcept;

    template<typename T>
    Result<Value> wrap(std::unique_ptr<T> native_obj) const noexcept;

    static Value fmap_to_value(const Object&) noexcept;
    static napi_value fmap_to_napi(const Object&) noexcept;
private:
    napi_env m_env;
    napi_value m_value;
};


//
// implementation
//
template<typename T>
Result<Value> Object::wrap(std::unique_ptr<T> native_obj) const noexcept {
    auto dtor = [](napi_env, void *data, void *) {
        delete static_cast<T *>(data);
    };
    if (auto status = napi_wrap(m_env, m_value, native_obj.get(), dtor, nullptr, nullptr); status != napi_ok) {
        return make_error_code(status);
    }
    native_obj.release();
    return to_value();
}

inline Value Object::to_value() const noexcept {
    return Value(m_env, m_value);
}

inline Value Object::fmap_to_value(const Object& obj) noexcept {
    return obj.to_value();
}

inline napi_value Object::fmap_to_napi(const Object& obj) noexcept {
    return obj.to_value().to_napi();
}

}
