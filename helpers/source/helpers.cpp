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
#include "helpers.hpp"

#include <fstream>
#include <sstream>

namespace helpers {

std::string readFile(std::string_view fileName) {
    std::ifstream fs(fileName.data());

    std::stringstream buffer;
    buffer << fs.rdbuf();

    return std::move(buffer).str();
}

} // helpers