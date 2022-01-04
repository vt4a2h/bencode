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

#include <bencode/element/types.hpp>

namespace bencode {

//! Bencode \p elements
[[nodiscard]]
std::string encode(const Elements &elements);

} // namespace bencode
