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
#include <string>
#include <vector>
#include <map>
#include <utility>

namespace bencode {

struct Element;

//! Type of decoded element
struct Type
{
    using Null = std::monostate; //< Auxiliary type (typically, you don't need to use it)
    using Int = std::int64_t; //< Corresponds to bencoded int
    using String = std::string; //< Corresponds to bencoded bytes sequence
    using List = std::vector<Element>; //< Corresponds to bencoded list
    using Dict = std::map<String, Element>; //< Corresponds to bencoded dict
};

namespace details {

using BaseElementType = std::variant<Type::Null, Type::Int, Type::String, Type::List, Type::Dict>;

} // details

//! Decoded element
struct Element : public details::BaseElementType {
    using details::BaseElementType::variant;

    template <class T> [[nodiscard]]
    inline constexpr bool is() const { return std::holds_alternative<T>(*this); }

    template <class T>
    inline constexpr T &as() & { return std::get<T>(*this); }

    template <class T>
    inline constexpr const T &as() const & { return std::get<T>(*this); }

    template <class T>
    inline constexpr T &&as() && { return std::get<T>(std::move(*this)); }

    template <class T>
    inline constexpr const T &&as() const && { return std::get<T>(*this); }
};

} // namespace bencode