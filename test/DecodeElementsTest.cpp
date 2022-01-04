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

#include <numeric>

#include <bencode/bencode.hpp>

TEST_CASE("Decode valid elements")
{
    namespace bc = bencode;

    auto [inputString, expectedElementVal] =
        GENERATE(
            // Int
            std::make_tuple(std::string("i42e"), bc::Element(42)),
            std::make_tuple(std::string("i-42e"), bc::Element(-42)),
            std::make_tuple(std::string("i-1e"), bc::Element(-1)),
            std::make_tuple(std::string("i-100e"), bc::Element(-100)),
            std::make_tuple(std::string("i0e"), bc::Element(0)),
            std::make_tuple(std::string("i101e"), bc::Element(101)),
            std::make_tuple(std::string("i100e"), bc::Element(100)),
            std::make_tuple("i" + std::to_string(std::numeric_limits<bc::Type::Int>::max()) + "e",
                            bc::Element(std::numeric_limits<bc::Type::Int>::max())),
            // Str
            std::make_tuple(std::string("0:"), bc::Element("")),
            std::make_tuple(std::string("1:a"), bc::Element("a")),
            std::make_tuple(std::string("5:abcde"), bc::Element("abcde")),
            std::make_tuple(std::string("10:aaaaaaaaaa"), bc::Element("aaaaaaaaaa")),
            std::make_tuple(std::string("11:aaaaaaaaaaa"), bc::Element("aaaaaaaaaaa")),

            // List
            std::make_tuple(std::string("le"), bc::Type::List{}),
            std::make_tuple(std::string("li42ee"), bc::Type::List{bc::Element(42)}),
            std::make_tuple(std::string("li42ei42ei42ee"), bc::Type::List{42, 42, 42}),
            std::make_tuple(std::string("l1:a1:a1:a1:a1:ae"), bc::Type::List{"a", "a", "a", "a", "a",}),

            // Nested lists
            std::make_tuple(std::string("llleleelee"),
                            bc::Type::List{bc::Type::List{bc::Type::List{}, bc::Type::List{}}, bc::Type::List{}}),
            std::make_tuple(std::string("llli42ei42eeli42eeeli42eee"),
                            bc::Type::List{bc::Type::List{bc::Type::List{42, 42}, bc::Type::List{42}},
                                           bc::Type::List{42}}),
            std::make_tuple(std::string("lll2:aael1:a1:aeel3:aaaee"),
                            bc::Type::List{bc::Type::List{bc::Type::List{"aa"}, bc::Type::List{"a", "a"}},
                                           bc::Type::List{"aaa"}}),

            // Dicts
            std::make_tuple(std::string("de"), bc::Type::Dict{}),
            std::make_tuple(std::string("d1:a1:be"), bc::Type::Dict{{"a", "b"}}),
            std::make_tuple(std::string("d1:ai42ee"), bc::Type::Dict{{"a", 42}}),
            std::make_tuple(std::string("d4:spaml1:a1:bee"), bc::Type::Dict{{"spam", bc::Type::List{"a", "b"}}}),

            // Nested dicts
            std::make_tuple(std::string("d1:ad1:adeee"), bc::Type::Dict{{"a", bc::Type::Dict{{"a", bc::Type::Dict{}}}}})
            );

    auto elementsResult = bc::decode(inputString);

    SECTION("Scanned without errors") {
        REQUIRE(elementsResult.hasValue());
    }

    auto elements = elementsResult.value();

    SECTION("Scanned as a single element") {
        REQUIRE(elements.size() == 1);
    }

    SECTION("Value can be obtained") {
        REQUIRE(elements.front() == bc::Element{expectedElementVal});
    }
}

TEST_CASE("Decode invalid elements") {
    namespace bc = bencode;

    auto inputString = GENERATE(
        // Int
        std::string("ie"),
        std::string("i e"),
        std::string("i 1ace"),
        std::string("i-e"),
        std::string("i001e"),
        std::string("i-0e"),
        "i" + std::to_string(std::numeric_limits<bc::Type::Int>::max()) + "0e",

        // Str
        std::string("0"),
        std::string("42"),
        std::string("12:"),
        std::string("12:ab"),
        std::string("01:a"),
        std::string("100e-1:a"),

        // List
        std::string("l e"),
        std::string("l"),
        std::string("e"),

        // Nested lists
        std::string("lle"),
        std::string("lleel")
        );

    auto elementsResult = bc::decode(inputString);

    SECTION("Result has an error") {
        REQUIRE(elementsResult.hasError());
    }

    SECTION("Error has valid type") {
        REQUIRE(elementsResult.error().type == bencode::ErrorType::DecodingError);
    }

    SECTION("Error has description") {
        REQUIRE(!elementsResult.error().description.empty());
    }
}