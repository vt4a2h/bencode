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

#include <variant>
#include <cassert>

#include "error.hpp"

#include <result/result.hpp>

namespace bencode {

template <class Value>
using Expected = result::Result<Value, Error>;

} // namespace bencode