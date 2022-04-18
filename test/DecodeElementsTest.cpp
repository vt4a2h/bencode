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

struct ExtendedElement : public bencode::Element {
    using bencode::Element::Element;

    Element &withRawData(std::string_view rawData) {
        raw = rawData;
        return *this;
    }
};

static const auto &bencodedIntMax() {
    static std::string data = "i" + std::to_string(std::numeric_limits<bencode::Type::Int>::max()) + "e";
    return data;
}

template <class ...Args>
static ExtendedElement makeList(Args &&...args) {
    return bencode::Type::List({std::forward<Args>(args)...});
}

static ExtendedElement makeInt(bencode::Type::Int val) {
    return val;
}

static ExtendedElement makeString(bencode::Type::String val) {
    return val;
}

static ExtendedElement makeDict(const std::vector<std::pair<bencode::Type::String, bencode::Element>> &data) {
    bencode::Type::Dict result;

    for (auto &&p : data)
        result.insert(p);

    return result;
}

TEST_CASE("Decode valid elements")
{
    namespace bc = bencode;

    auto [inputString, expectedElementVal] =
        GENERATE(
            // Int
            std::make_tuple(std::string("i42e"), ExtendedElement(42).withRawData("i42e")),
            std::make_tuple(std::string("i-42e"), ExtendedElement(-42).withRawData("i-42e")),
            std::make_tuple(std::string("i-1e"), ExtendedElement(-1).withRawData("i-1e")),
            std::make_tuple(std::string("i-100e"), ExtendedElement(-100).withRawData("i-100e")),
            std::make_tuple(std::string("i0e"), ExtendedElement(0).withRawData("i0e")),
            std::make_tuple(std::string("i101e"), ExtendedElement(101).withRawData("i101e")),
            std::make_tuple(std::string("i100e"), ExtendedElement(100).withRawData("i100e")),
            std::make_tuple(bencodedIntMax(),
                            ExtendedElement(std::numeric_limits<bc::Type::Int>::max()).withRawData(bencodedIntMax())),
            // Str
            std::make_tuple(std::string("0:"), ExtendedElement("").withRawData("0:")),
            std::make_tuple(std::string("1:a"), ExtendedElement("a").withRawData("1:a")),
            std::make_tuple(std::string("5:abcde"), ExtendedElement("abcde").withRawData("5:abcde")),
            std::make_tuple(std::string("10:aaaaaaaaaa"), ExtendedElement("aaaaaaaaaa").withRawData("10:aaaaaaaaaa")),
            std::make_tuple(std::string("11:aaaaaaaaaaa"), ExtendedElement("aaaaaaaaaaa").withRawData("11:aaaaaaaaaaa")),

            // List
            std::make_tuple(std::string("le"), makeList().withRawData("le")),
            std::make_tuple(std::string("li42ee"), makeList(makeInt(42).withRawData("i42e")).withRawData("li42ee")),
            std::make_tuple(std::string("li42ei42ei42ee"),
                            makeList(makeInt(42).withRawData("i42e"),
                                     makeInt(42).withRawData("i42e"),
                                     makeInt(42).withRawData("i42e")).withRawData("li42ei42ei42ee")),
            std::make_tuple(std::string("l1:a1:a1:a1:a1:ae"),
                            makeList(makeString("a").withRawData("1:a"),
                                     makeString("a").withRawData("1:a"),
                                     makeString("a").withRawData("1:a"),
                                     makeString("a").withRawData("1:a"),
                                     makeString("a").withRawData("1:a")).withRawData("l1:a1:a1:a1:a1:ae")),

            // Nested lists
            std::make_tuple(std::string("llleleelee"),
                            makeList(makeList(makeList().withRawData("le"),
                                              makeList().withRawData("le")).withRawData("llelee"),
                                     makeList().withRawData("le")).withRawData("llleleelee")),
            std::make_tuple(std::string("llli42ei42eeli42eeeli42eee"),
                            makeList(makeList(makeList(makeInt(42).withRawData("i42e"),
                                                       makeInt(42).withRawData("i42e")).withRawData("li42ei42ee"),
                                              makeList(makeInt(42).withRawData("i42e")).withRawData("li42ee")).withRawData("lli42ei42eeli42eee"),
                                     makeList(makeInt(42).withRawData("i42e")).withRawData("li42ee")).withRawData("llli42ei42eeli42eeeli42eee")),
            std::make_tuple(std::string("lll2:aael1:a1:aeel3:aaaee"),
                            makeList(makeList(makeList(makeString("aa").withRawData("2:aa")).withRawData("l2:aae"),
                                              makeList(makeString("a").withRawData("1:a"),
                                                       makeString("a").withRawData("1:a")).withRawData("l1:a1:ae")).withRawData("ll2:aael1:a1:aee"),
                                     makeList(makeString("aaa").withRawData("3:aaa")).withRawData("l3:aaae")).withRawData("lll2:aael1:a1:aeel3:aaaee")),

            // Dicts
            std::make_tuple(std::string("de"), makeDict({}).withRawData("de")),
            std::make_tuple(std::string("d1:a1:be"), makeDict({
                {makeString("a").withRawData("1:a").as<bencode::Type::String>(), makeString("b").withRawData("1:b")}
            }).withRawData("d1:a1:be")),
            std::make_tuple(std::string("d1:ai42ee"), makeDict({
                {makeString("a").withRawData("1:a").as<bencode::Type::String>(), makeInt(42).withRawData("i42e")}
            }).withRawData("d1:ai42ee")),
            std::make_tuple(std::string("d4:spaml1:a1:bee"), makeDict({
                {makeString("spam").withRawData("4:spam").as<bencode::Type::String>(),
                 makeList(makeString("a").withRawData("1:a"),
                          makeString("b").withRawData("1:b")).withRawData("l1:a1:be")}
            }).withRawData("d4:spaml1:a1:bee")),

            // Nested dicts
            std::make_tuple(std::string("d1:ad1:adeee"), makeDict({
                {makeString("a").withRawData("1:a").as<bencode::Type::String>(), makeDict({
                    {makeString("a").withRawData("1:a").as<bencode::Type::String>(), makeDict({}).withRawData("de")},
                }).withRawData("d1:adee")}
            }).withRawData("d1:ad1:adeee"))
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
        REQUIRE(elements.front() == expectedElementVal);
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