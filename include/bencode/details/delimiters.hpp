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

namespace bencode::details {

inline constexpr const char IntStartDelim = 'i';
inline constexpr const char ListStartDelim = 'l';
inline constexpr const char DictStartDelim = 'd';

inline constexpr const char EndDelim = 'e';

inline constexpr const char StringDelim = ':';

} // namespace bencode::details
