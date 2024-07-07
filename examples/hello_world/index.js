// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// Hello world example of craftnapi
//

const hello_world = require('./build/Release/hello_world');

console.log(hello_world.hello_world("Craftnapi!"));

try {
    console.log(hello_world.hello_world());
} catch (e) {
    console.log("Expected error:", e.message);
}
