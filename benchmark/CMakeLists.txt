find_package(Catch2 CONFIG REQUIRED)

set(SOURCE_LIST DecodeFileBenchmark.cpp EncodeFileBenchmark.cpp)

add_executable(benchmarks main.cpp ${SOURCE_LIST})

target_compile_features(benchmarks PRIVATE cxx_std_20)

target_link_libraries(benchmarks PRIVATE bencode bencode_helpers Catch2::Catch2 pthread)

add_custom_command(TARGET benchmarks PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/data/*.torrent
        ${CMAKE_CURRENT_BINARY_DIR}/)

include(Catch)
catch_discover_tests(benchmarks)
