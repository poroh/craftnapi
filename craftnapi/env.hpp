//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Environment
//

#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "craftnapi/error.hpp"
#include "craftnapi/common.hpp"
#include "craftnapi/value.hpp"
#include "craftnapi/array.hpp"
#include "craftnapi/object.hpp"
#include "craftnapi/callback_info.hpp"

namespace craftnapi {

class Env {
public:
    explicit Env(napi_env);

    Result<CallbackInfo> create_callback_info(napi_callback_info, void ** = nullptr) const noexcept;

    using RVV = Result<Value>;
    using RVO = Result<Object>;
    using RVA = Result<Array>;

    using ValueInit = std::variant<Value, RVV, Maybe<RVV>, Object, RVO, Maybe<RVO>>;
    using ObjectSpec = std::vector<std::pair<std::string, ValueInit>>;
    using Function = std::function<RVV(Env& env, const CallbackInfo&)>;

    RVO create_object() const noexcept;
    RVO create_object(const ObjectSpec&) const noexcept;
    napi_value exports(const ObjectSpec&) const noexcept;

    RVA create_array() const noexcept;

    template<typename Container, typename F>
    RVV create_array(const Container&, F&&);

    RVV create_undefined() const noexcept;
    RVV create_null() const noexcept;
    RVV create_string(const std::string_view&) const noexcept;
    RVV create_buffer(const ConstBinaryView& view) const noexcept;
    RVV create_boolean(bool value) const noexcept;
    RVV create_int32(int32_t value) const noexcept;
    RVV create_uint32(int32_t value) const noexcept;
    RVV create_bigint_uint64(uint64_t value) const noexcept;
    RVV create_function(Function, Maybe<std::string_view> name = craftpp::maybe::none());
    RVV throw_error(const std::string& message) const noexcept;

    struct ClassAttr {
        Maybe<Function> getter;
        Maybe<Function> setter;
    };
    using ClassProperty = std::variant<Function, RVV, ClassAttr>;
    using ClassPropertySpec = std::vector<std::pair<std::string, ClassProperty>>;
    RVV create_class(std::string_view name, Function ctor, const ClassPropertySpec&) const noexcept;

    template<typename T>
    Maybe<T> maybe_throw_error(const Result<T>& v) const noexcept;
private:
    napi_env m_env;
};

//
// implementation
//
template<typename T>
Maybe<T> Env::maybe_throw_error(const Result<T>& v) const noexcept {
    return v
        .fmap([](auto&& v) {
            return Maybe<T>{v};
        })
        .value_or_call([&](auto&& err) {
            throw_error(err.message());
            return Maybe<T>::none();
        });
}

template<typename Container, typename F>
Env::RVV Env::create_array(const Container& c, F&& f) {
    return create_array()
        > [&](auto&& array) -> RVA {
            size_t i = 0;
            for (const auto& v: c) {
                auto maybe_err = f(v)
                    .bind([&](auto&& napiv) {
                        return array.set_element(i++, napiv);
                    });
                if (maybe_err.is_err()) {
                    return maybe_err.unwrap_err();
                }
            }
            return array;
        }
        > Array::fmap_to_value;
}

}
