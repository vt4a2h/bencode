set(SOURCE_LIST helpers.cpp)

add_library(bencode_helpers ${SOURCE_LIST})

target_compile_features(bencode_helpers PUBLIC cxx_std_20)