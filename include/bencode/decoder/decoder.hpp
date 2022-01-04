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

#include <bencode/element/element.hpp>
#include <bencode/result/expected.hpp>
#include <bencode/element/types.hpp>

namespace bencode {

//! Decode \p input.
//! \note \p input is a view. Object it refers to should live long enough.
//! \return vector of elements or \p DecodingError.
[[nodiscard]]
Expected<Elements> decode(std::string_view input);

} // namespace bencode