// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// Create class example of craftnapi
//

#include <iostream>

#include <craftnapi/env.hpp>

using craftnapi::Result;

struct Options {
    std::string prefix;
    std::string suffix;
};

class MyClass {
public:
    MyClass(Options options) : m_options(options) {}
    std::string say(const std::string& what) const {
        return m_options.prefix + what + m_options.suffix;
    }
private:
    Options m_options;
};

Result<Options> parse_opts(const craftnapi::Object& obj) {
    auto prefix_r = obj.maybe_named_property("prefix")
        .bind([](auto v) { return v.require(); })
        .bind(craftnapi::Value::to_string);
    auto suffix_r = obj.maybe_named_property("suffix")
        .bind([](auto v) { return v.require(); })
        .bind(craftnapi::Value::to_string);
    return combine([](auto&& prefix, auto& suffix) -> Result<Options> {
        return Options {
            prefix,
            suffix
        };
    }, prefix_r, suffix_r);
}

Result<craftnapi::Value> constructor(craftnapi::Env& env, const craftnapi::CallbackInfo& ci) {
    auto options_r = ci[0]
        .add_context("first argument")
        .bind(craftnapi::Value::to_object)
        .bind(parse_opts);
    return
        combine([](auto&& obj, auto&& options) {
            return obj.wrap(std::make_unique<MyClass>(options));
        },
        ci.this_arg.as_object(),
        options_r);
}


Result<craftnapi::Value> say(craftnapi::Env& env, const craftnapi::CallbackInfo& ci) {
    auto self_r = ci.this_arg.unwrap<MyClass>();
    auto what_r = ci[0].bind(craftnapi::Value::to_string);
    return
        combine([&](auto&& self, auto&& what) {
            return env.create_string(self.get().say(what));
        }, self_r, what_r);
}


napi_value init(napi_env inenv, napi_value) {
    craftnapi::Env env(inenv);
    const auto name = "MyClass";
    return env.exports({
        { name,
          env.create_class(
              name,
              constructor,
              {
                  {"say", say},
              })
        },
    });
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
