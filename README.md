# Craftnapi

This library is another wrapper for node.js C++ bindings.
How is it different?

- It is written over napi interface that supposed to be stable in long term
- It reuses paradigms from functional programming to reduce return code checking

Minimal example of native module:

```cpp
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
```

And this code checks all return codes returned by napi interface!
