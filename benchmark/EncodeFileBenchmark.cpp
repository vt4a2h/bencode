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
#define CATCH_CONFIG_USE_ASYNC
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <bencode/bencode.hpp>

#include "helpers.hpp"

TEST_CASE("Encode file benchmark")
{
    namespace bc = bencode;

    auto fileName = GENERATE("open_suse_15_3.torrent", "sts_2.torrent", "mtr_rl.torrent", "mtr_rl_10x.torrent");
    auto data = helpers::readFile(fileName);;
    auto decodedResult = bc::decode(data);
    auto decodedValue = decodedResult.value();

    BENCHMARK(std::string("Encoding \"") + fileName + "\"") {
        return bc::encode(decodedValue);
    };
}