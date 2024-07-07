//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI Environment
//

#include <craftpp/result/reduce.hpp>
#include <craftpp/variant/overloaded.hpp>

#include "craftnapi/env.hpp"
#include "craftnapi/error.hpp"

namespace craftnapi {

Env::Env(napi_env env)
    : m_env(env)
{}

Result<CallbackInfo> Env::create_callback_info(napi_callback_info info, void **data) const noexcept {
    size_t argc = 0;
    napi_value this_arg;
    if (const auto status = napi_get_cb_info(m_env, info, &argc, nullptr, &this_arg, data); status != napi_ok) {
        return make_error_code(status);
    }

    std::vector<napi_value> args(argc);
    if (auto status = napi_get_cb_info(m_env, info, &argc, args.data(), nullptr, nullptr); status != napi_ok) {
        return make_error_code(status);
    }

    std::vector<Value> result_args;
    result_args.reserve(argc);
    for (const auto& v: args) {
        result_args.emplace_back(m_env, v);
    }
    return CallbackInfo(Value{m_env, this_arg}, std::move(result_args));
}

Result<Object> Env::create_object() const noexcept {
    napi_value obj;
    if (auto status = napi_create_object(m_env, &obj); status != napi_ok) {
        return make_error_code(status);
    }
    return Object(m_env, obj);
}

Result<Object> Env::create_object(const ObjectSpec& spec) const noexcept {
    return create_object()
        .bind([&](Object&& object) {
            return craftpp::result::reduce(spec.begin(), spec.end(), object, [](Object&& object, auto&& p) {
                const auto& k = p.first;
                const auto& v = p.second;
                using MaybeRVV = Maybe<Result<Value>>;
                using MaybeRVO = Maybe<Result<Object>>;
                auto mrvv = std::visit(
                    craftpp::variant::overloaded {
                        [](const Result<Value>& rv) -> MaybeRVV { return rv; },
                        [](const Value& vv) -> MaybeRVV { return Result<Value>{vv}; },
                        [](const MaybeRVV& mrvv) { return mrvv; },
                        [](const Object& vv)  -> MaybeRVV { return Result<Value>{vv.to_value()}; },
                        [](const RVO& rv) -> MaybeRVV { return rv.fmap([](const auto& obj) { return obj.to_value(); }); },
                        [](const MaybeRVO& mrvo) -> MaybeRVV {
                             return mrvo.fmap([](auto&& rv) {
                                return rv.fmap(Object::fmap_to_value);
                             });
                        }
                    }, v);
                return std::move(mrvv)
                    .fmap([&](Result<Value>&& rvv) -> Result<Object> {
                        return std::move(rvv)
                            .bind([&](Value&& vv) { return object.set_named_property(k, vv); })
                            .fmap([&](auto&&) { return object; })
                            .add_context(k + " attribute");
                    })
                    .value_or(Result<Object>{object});
            });
        });
}

Result<Array> Env::create_array() const noexcept {
    napi_value arr;
    if (auto status = napi_create_array(m_env, &arr); status != napi_ok) {
        return make_error_code(status);
    }
    return Array(m_env, arr);
}

Result<Value> Env::create_undefined() const noexcept {
    napi_value result;
    if (auto status = napi_get_undefined(m_env, &result); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, result);
}

Result<Value> Env::create_null() const noexcept {
    napi_value result;
    if (auto status = napi_get_null(m_env, &result); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, result);
}

Result<Value> Env::create_string(const std::string_view& str) const noexcept {
    napi_value result;
    if (auto status = napi_create_string_utf8(m_env, str.data(), str.size(), &result); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, result);
}

Result<Value> Env::create_buffer(const ConstBinaryView& view) const noexcept {
    napi_value buffer;
    void *data;
    if (napi_status status = napi_create_buffer(m_env, view.size(), &data, &buffer); status != napi_ok) {
        return make_error_code(status);
    }
    memcpy(data, view.data(), view.size());
    return Value(m_env, buffer);
}

Result<Value> Env::create_boolean(bool value) const noexcept {
    napi_value boolean;
    if (napi_status status = napi_get_boolean(m_env, value, &boolean); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, boolean);
}

Result<Value> Env::create_int32(int32_t value) const noexcept {
    napi_value int32;
    if (napi_status status = napi_create_int32(m_env, value, &int32); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, int32);
}

Result<Value> Env::create_uint32(int32_t value) const noexcept {
    napi_value uint32;
    if (napi_status status = napi_create_uint32(m_env, value, &uint32); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, uint32);
}

Result<Value> Env::create_bigint_uint64(uint64_t value) const noexcept {
    napi_value result;
    if (napi_status status = napi_create_bigint_uint64(m_env, value, &result); status != napi_ok) {
        return make_error_code(status);
    }
    return Value(m_env, result);
}

Result<Value> Env::throw_error(const std::string& message) const noexcept {
    if (napi_status status = napi_throw_error(m_env, nullptr, message.c_str()); status != napi_ok) {
        return make_error_code(status);
    }
    return create_undefined();
}

static napi_value function_wrapper(napi_env inenv, napi_callback_info info) {
    Env env(inenv);
    void *data;
    auto ci_info_rvv = env.create_callback_info(info, &data);
    return env.maybe_throw_error(ci_info_rvv)
        .bind([&](auto&& ci_info) {
            auto& f = *static_cast<Env::Function *>(data);
            return env.maybe_throw_error(f(env, ci_info));
        })
        .fmap([](auto&& ret) {
            return ret.to_napi();
        })
        .value_or(nullptr);
}

Result<Value> Env::create_function(Function f, Maybe<std::string_view> maybename) {
    napi_value result;
    auto data = std::make_unique<Function>(std::move(f));
    if (maybename.is_some()) {
        const auto& name = maybename.unwrap();
        if (auto status = napi_create_function(m_env, name.data(), name.size(), function_wrapper, data.get(), &result); status != napi_ok) {
            return make_error_code(status);
        }
    } else {
        if (auto status = napi_create_function(m_env, nullptr, 0, function_wrapper, data.get(), &result); status != napi_ok) {
            return make_error_code(status);
        }
    }
    auto dtor = [](napi_env, void *data, void *) {
        delete static_cast<Function *>(data);
    };
    if (auto status = napi_add_finalizer(m_env, result, data.get(), dtor, nullptr, nullptr); status != napi_ok) {
        return make_error_code(status);
    }
    data.release();
    return Value(m_env, result);
}

static napi_value function_getter(napi_env inenv, napi_callback_info info) {
    Env env(inenv);
    void *data;
    auto ci_info_rvv = env.create_callback_info(info, &data);
    return env.maybe_throw_error(ci_info_rvv)
        .bind([&](auto&& ci_info) {
            auto& attr = *static_cast<Env::ClassAttr *>(data);
            return attr.getter
                .fmap([&](auto&& getter) {
                    return getter(env, ci_info);
                });
        })
        .bind([&](auto&& rv) {
            return env.maybe_throw_error(rv);
        })
        .fmap(Value::fmap_to_napi)
        .value_or(nullptr);
}

static napi_value function_setter(napi_env inenv, napi_callback_info info) {
    Env env(inenv);
    void *data;
    auto ci_info_rvv = env.create_callback_info(info, &data);
    return env.maybe_throw_error(ci_info_rvv)
        .bind([&](auto&& ci_info) {
            auto& attr = *static_cast<Env::ClassAttr *>(data);
            return attr.setter
                .fmap([&](auto&& setter) {
                    return setter(env, ci_info);
                });
        })
        .bind([&](auto&& rv) {
            return env.maybe_throw_error(rv);
        })
        .fmap(Value::fmap_to_napi)
        .value_or(nullptr);
}

struct DescrRVVisitor {
    using DescrRV = Result<napi_property_descriptor>;

    const Env::ClassPropertySpec::value_type& p;

    auto operator()(const Env::Function& f) -> DescrRV {
        return napi_property_descriptor{
            p.first.c_str(),
            nullptr, // name
            function_wrapper, // method
            nullptr, // getter
            nullptr, // setter
            nullptr, // value
            napi_default, // attributes
            new Env::Function(f) // data
        };
    }
    auto operator()(const Result<Value>& rvv) -> DescrRV {
        return rvv.fmap([&](auto&& v) {
            return napi_property_descriptor{
                p.first.c_str(),
                nullptr, // name
                function_wrapper, // method
                nullptr, // getter
                nullptr, // setter
                v.to_napi(), // value
                napi_default, // attributes
                nullptr  // data
            };
        });
    }
    auto operator()(const Env::ClassAttr& attr) -> DescrRV {
        return napi_property_descriptor{
            p.first.c_str(),
            nullptr, // name
            nullptr, // method
            function_getter, // getter
            function_setter, // setter
            nullptr, // value
            napi_default, // attributes
            new Env::ClassAttr(attr)  // data
        };
    }
};

Result<Value> Env::create_class(std::string_view name, Function ctor, const ClassPropertySpec& spec) const noexcept {
    using DescVec = std::vector<napi_property_descriptor>;
    return
        craftpp::result::reduce(spec.begin(), spec.end(), DescVec{}, [](DescVec&& dv, const auto& p) {
            return std::visit(DescrRVVisitor{p}, p.second)
                .add_context("attribute create", p.first)
                .fmap([&](auto&& v) {
                    dv.emplace_back(std::move(v));
                    return std::move(dv);
                });
        })
        .bind([&](DescVec&& dv) -> Result<Value> {
            napi_value result;
            auto status =
                napi_define_class(
                    m_env,
                    name.data(),
                    name.size(),
                    function_wrapper,
                    new Function(ctor),
                    dv.size(),
                    dv.data(),
                    &result);
            if (status != napi_ok) {
                return make_error_code(status);
            }
            // Note that data in property descriptions data will leak
            // if module can be uploaded. We can add napi_add_finalizer to
            // class definition as well but it is not practal thing.
            return Value(m_env, result);
        });
}

}
