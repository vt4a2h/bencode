//
// MIT License
//
// Copyright (c) 2022-present Vitaly Fanaskov
//
// bencode -- A simple library for decoding bencoded data
// Project home: https://github.com/vt4a2h/bencode
//
// See LICENSE file for the further details.
//
#pragma once

#include <string>

namespace bencode {

//! Type for errors
enum class ErrorType {
    None,
    DecodingError, //!< Some error occurred during decoding bencoded data
};

//! Error type + description
struct Error {
    ErrorType type;
    std::string description;
};

}
