// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// Hello world example of craftnapi
//

#include <node_api.h>
#include <craftnapi/env.hpp>

using craftnapi::Result;

Result<craftnapi::Value> hello_world(craftnapi::Env& env, const craftnapi::CallbackInfo& ci) {
    return ci[0]
        .bind([](auto&& val) {
            return val.as_string();
        })
        .bind([&](auto&& str) {
            return env.create_string("Hello world: " + str);
        });
}

napi_value init(napi_env inenv, napi_value) {
    craftnapi::Env env(inenv);
    auto exports =
        env.create_object({
                { "hello_world", env.create_function(hello_world) },
            });
    return env.maybe_throw_error(exports)
        .fmap([](auto&& obj) {
            return obj.to_value().to_napi();
        })
        .value_or(nullptr);
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
