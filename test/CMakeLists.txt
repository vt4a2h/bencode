find_package(Catch2 CONFIG REQUIRED)

set(SOURCE_LIST
    DecodeElementsTest.cpp
    DecodeFileTest.cpp
    EncodeElementsTest.cpp
    EncodeFileTest.cpp)

add_executable(tests
    main.cpp
    ${SOURCE_LIST}
)

target_compile_features(tests PRIVATE cxx_std_20)

target_link_libraries(tests PRIVATE bencode bencode_helpers Catch2::Catch2)

add_custom_command(TARGET tests PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/data/*.torrent
            ${CMAKE_CURRENT_BINARY_DIR}/)

include(Catch)
catch_discover_tests(tests)
