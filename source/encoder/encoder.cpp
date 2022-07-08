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
#include <bencode/encoder/encoder.hpp>

#include <sstream>
#include <algorithm>

#include <bencode/element/element.hpp>
#include <bencode/details/delimiters.hpp>

namespace bencode {

using namespace details;

void encodeElem(std::stringstream &ss, const Element &element);

void encodeInt(std::stringstream &ss, Type::Int value) {
    ss << IntStartDelim << value << EndDelim;
}

void encodeString(std::stringstream &ss, const Type::String &str) {
    ss << str.size() << StringDelim << str;
}

void encodeList(std::stringstream &ss, const Type::List &lst) {
    ss << ListStartDelim;
    std::ranges::for_each(lst, [&](const auto &e) { encodeElem(ss, e); });
    ss << EndDelim;
}

void encodeDict(std::stringstream &ss, const Type::Dict &dict) {
    ss << DictStartDelim;
    std::ranges::for_each(dict, [&](const auto &p) { encodeString(ss, p.first); encodeElem(ss, p.second); });
    ss << EndDelim;
}

// Do not rush with improving this, please!
// https://quick-bench.com/q/LhdP-9y6CqwGxB-WtDlbG27o_5Y
void encodeElem(std::stringstream &ss, const Element &element) {
    switch (details::indexType(element)) {
        break; case details::BaseElementIndexType::Int: encodeInt(ss, element.as<Type::Int>());
        break; case details::BaseElementIndexType::String: encodeString(ss, element.as<Type::String>());
        break; case details::BaseElementIndexType::List: encodeList(ss, element.as<Type::List>());
        break; case details::BaseElementIndexType::Dict: encodeDict(ss, element.as<Type::Dict>());
        default: /*pass*/;
    }
}

std::string encode(const Elements &elements) {
    std::stringstream out;

    for (const auto &element: elements)
        encodeElem(out, element);

    return std::move(out).str();
}

} // namespace bencode
