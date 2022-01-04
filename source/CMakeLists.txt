set(SOURCE_LIST
    decoder/decoder.cpp
    encoder/encoder.cpp)

add_library(bencode
    ${SOURCE_LIST}
)

target_compile_features(bencode PUBLIC cxx_std_20)
