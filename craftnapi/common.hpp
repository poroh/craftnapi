//
// Copyright (c) 2024 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI common definitions
//
#pragma once

#include <node_api.h>
#include <craftpp/result/result.hpp>
#include <craftpp/maybe/maybe.hpp>
#include <craftpp/binary/const_view.hpp>

namespace craftnapi {

using MaybeError = craftpp::result::MaybeError;

template<typename T>
using Result = craftpp::result::Result<T>;

template<typename T>
using Maybe = craftpp::maybe::Maybe<T>;

using ConstBinaryView = craftpp::binary::ConstView;

}

