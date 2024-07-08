// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// Hello world example of craftnapi
//

#include <craftnapi/env.hpp>

craftnapi::Result<craftnapi::Value> hello_world(craftnapi::Env& env, const craftnapi::CallbackInfo& ci) {
    return ci[0]
        .add_context("first argument")
        .bind(craftnapi::Value::to_string)
        .bind([&](auto&& str) {
            return env.create_string("Hello world: " + str);
        });
}

napi_value init(napi_env inenv, napi_value) {
    craftnapi::Env env(inenv);
    return env.exports({
        { "hello_world", env.create_function(hello_world) },
    });
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
