//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Array
//
#pragma once

#include "craftnapi/common.hpp"
#include "craftnapi/value.hpp"

namespace craftnapi {

class Array {
public:
    Array(napi_env, napi_value);

    Value to_value() const noexcept;
    MaybeError set_element(size_t i, const Value&) noexcept;

    static Value fmap_to_value(const Array&) noexcept;
private:
    napi_env m_env;
    napi_value m_value;
};

//
// implementation
//
inline Value Array::to_value() const noexcept {
    return Value(m_env, m_value);
}

inline Value Array::fmap_to_value(const Array& arr) noexcept {
    return arr.to_value();
}

}
