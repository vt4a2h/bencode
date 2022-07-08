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
#include <compare>

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
enum class BaseElementIndexType { Null, Int, String, List, Dict };

inline constexpr BaseElementIndexType indexType(const details::BaseElementType &e) {
    switch (e.index()) {
        case 0: return BaseElementIndexType::Null;
        case 1: return BaseElementIndexType::Int;
        case 2: return BaseElementIndexType::String;
        case 3: return BaseElementIndexType::List;
        case 4: return BaseElementIndexType::Dict;
        default: return BaseElementIndexType::Null;
    }
}

} // details

/*! Decoded element
 *
 *  Always use \p is before you call \p as.
 *
 *  \note The lifetime of \p raw corresponds to the lifetime of a string passed
 *  to the \p decoder::decode. If you are not going to use \p raw at all,
 *  you don't have to care about it.
 */
struct Element : public details::BaseElementType {
    using details::BaseElementType::variant;

    template <class T> [[nodiscard]]
    inline constexpr bool is() const noexcept { return std::holds_alternative<T>(*this); }

    template <class T>
    inline constexpr T &as() & noexcept { return *std::get_if<T>(this); }

    template <class T>
    inline constexpr const T &as() const & noexcept { return *std::get_if<T>(this); }

    template <class T>
    inline constexpr T &&as() && noexcept { return std::move(*std::get_if<T>(this)); }

    template <class T>
    inline constexpr const T &&as() const && noexcept { return std::move(*std::get_if<T>(this)); }

    auto operator<=>(const Element&) const noexcept = default;

    std::string_view raw;
};

} // namespace bencode