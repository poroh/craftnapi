// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// Class creation example of craftnapi
//

const create_class = require('./build/Release/create_class');

let talker = new create_class.MyClass({
    prefix: "Hello ",
    suffix: "!"
});
console.log(talker.say("World"));
