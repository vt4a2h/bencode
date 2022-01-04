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
#include "catch.hpp"

#include <bencode/bencode.hpp>

namespace bc = bencode;

TEST_CASE("Encode valid elements") {
    auto [inputElement, expectedString] = GENERATE(
        // Int
        std::make_tuple(bc::Element(42), std::string("i42e")),
        std::make_tuple(bc::Element(-42), std::string("i-42e")),
        std::make_tuple(bc::Element(0), std::string("i0e")),
        std::make_tuple(bc::Element(101), std::string("i101e")),
        std::make_tuple(bc::Element(100), std::string("i100e")),
        std::make_tuple(bc::Element(std::numeric_limits<bc::Type::Int>::max()),
                        "i" + std::to_string(std::numeric_limits<bc::Type::Int>::max()) + "e"),

        // String
        std::make_tuple(bc::Element(""), std::string("0:")),
        std::make_tuple(bc::Element("a"), std::string("1:a")),
        std::make_tuple(bc::Element("abcde"), std::string("5:abcde")),

        // List
        std::make_tuple(bc::Type::List{}, std::string("le")),
        std::make_tuple(bc::Type::List{bc::Element(42)}, std::string("li42ee")),
        std::make_tuple(bc::Type::List{42, 42, 42}, std::string("li42ei42ei42ee")),
        std::make_tuple(bc::Type::List{"a", "a", "a", "a", "a",}, std::string("l1:a1:a1:a1:a1:ae")),

        // Nested lists
        std::make_tuple(bc::Type::List{bc::Type::List{bc::Type::List{}, bc::Type::List{}}, bc::Type::List{}},
                        std::string("llleleelee")),
        std::make_tuple(bc::Type::List{bc::Type::List{bc::Type::List{42, 42}, bc::Type::List{42}},
                                       bc::Type::List{42}},
                        std::string("llli42ei42eeli42eeeli42eee")),
        std::make_tuple(bc::Type::List{bc::Type::List{bc::Type::List{"aa"}, bc::Type::List{"a", "a"}},
                                       bc::Type::List{"aaa"}},
                        std::string("lll2:aael1:a1:aeel3:aaaee")),

        // Dicts
        std::make_tuple(bc::Type::Dict{}, std::string("de")),
        std::make_tuple(bc::Type::Dict{{"a", "b"}}, std::string("d1:a1:be")),
        std::make_tuple(bc::Type::Dict{{"a", 42}}, std::string("d1:ai42ee")),
        std::make_tuple(bc::Type::Dict{{"spam", bc::Type::List{"a", "b"}}}, std::string("d4:spaml1:a1:bee")),

         // Nested dicts
        std::make_tuple(bc::Type::Dict{{"a", bc::Type::Dict{{"a", bc::Type::Dict{}}}}}, std::string("d1:ad1:adeee"))
        );

    REQUIRE(bc::encode({inputElement,}) == expectedString);
}

TEST_CASE("Encode invalid elements") {
    REQUIRE(bc::encode({bc::Type::Null{},}).empty());
}
