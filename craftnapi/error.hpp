//
// Copyright (c) 2023 Dmitry Poroh
// All rights reserved.
// Distributed under the terms of the MIT License. See the LICENSE file.
//
// NAPI status to error_code
//

#pragma once

#include <system_error>
#include <node_api.h>

namespace craftnapi {

class NapiErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override;
    std::string message(int code) const override;
};

std::error_code make_error_code(napi_status status);

enum class WrapperError {
    ok = 0,
    invalid_type,
    no_required_argument
};

class NapiWrapperErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override;
    std::string message(int code) const override;
};

std::error_code make_error_code(WrapperError error);

}

