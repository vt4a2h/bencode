# Bencode

This is a library for working with encoded data commonly used in the torrent-files.
Both encoding and decoding operations are supported. 

Validation is performed according to the encoding algorithm
described here https://en.wikipedia.org/wiki/Bencode, http://www.bittorrent.org/beps/bep_0003.html. 

Lists are heterogeneous. Dictionaries are sorted by keys. Keys can only be a string, 
values are heterogeneous. A single key matches a single value. These data structures 
can be nested. For example, it's possible to have a list of lists or a dictionary with 
another dictionary as a value of some key(s).

You need a compiler with at least C++20 support to build the library.

# Examples

## Decoding
Decoding a file can be done as follows:
```c++
auto data = readFile(fileName);
if (auto result = bencode::decode(data); result.hasValue()) {
    // Vector of the top-level elements discovered in the file:
    // std::vector<Element>
    auto elements = result.value();
    for (const auto &element: elements) {
        if (element.is<bencode::Type::Dict>()) {
            // std::map<std::string, bencode::Element>
            auto dict = element.as<bencode::Type::Dict>();
        }
        
        // ... 
    }
} else {
    auto err = result.error();
    std::cout << "decoding error: " << err.description << std::endl;
}
```
*Note*: `bencode::decode` takes `std::string_view` as an input parameter. Beware of using temporary objects.

*Note*: `bencode::Element` is essentially `std::variant`. You can use `std::visit` for matching.

## Encoding

Encoding elements can be done as follows:
```c++
namespace bc = bencode;

auto element = bc::Type::Dict{{"spam", bc::Type::List{"a", "b"}}};
auto result = bc::encode({element, /*other elements*/}); 
// result == "d4:spaml1:a1:bee"
```
There is no special error handling for encoding. The result is always a string. 

*Note*: the result of `bc::encode` is empty for `Type::Null`.

# Build

The default building:
```shell
mkdir build
cd build
cmake -G "Ninja" ..
cmake --build . --target bencode
```

With tests:
```shell
cmake -G "Ninja" .. -D BUILD_TESTS=ON
cmake --build .
ctest
```
You can expect a similar output:
```shell
Test project bencode/build
    Start 1: Decode valid elements
1/6 Test #1: Decode valid elements ............   Passed    0.00 sec
    Start 2: Decode invalid elements
2/6 Test #2: Decode invalid elements ..........   Passed    0.00 sec
    Start 3: Decoded file is valid
3/6 Test #3: Decoded file is valid ............   Passed    0.11 sec
    Start 4: Encode valid elements
4/6 Test #4: Encode valid elements ............   Passed    0.00 sec
    Start 5: Encode invalid elements
5/6 Test #5: Encode invalid elements ..........   Passed    0.00 sec
    Start 6: Encode file is valid
6/6 Test #6: Encode file is valid .............   Passed    0.13 sec

100% tests passed, 0 tests failed out of 6

Total Test time (real) =   0.25 sec
```

You need to have Catch2 framework installed for testing. One way of doing this is 
by using a package manager. For `vcpkg`, simple pass `CMAKE_TOOLCHAIN_FILE` to the `cmake` command:
```shell
cmake -G "Ninja" .. -D CMAKE_TOOLCHAIN_FILE=<path to vcpkg>/vcpkg/scripts/buildsystems/vcpkg.cmake
```

With benchmarks:
```shell
cmake -G "Ninja" .. -D CMAKE_BUILD_TYPE=Release -D BUILD_BENCHMARKS=ON
cmake --build .
ctest
```
You can invoke `ctest` with `--extra-verbose` flag to get output like this:
```shell
7: Decode file benchmark
7: -------------------------------------------------------------------------------
7: ../benchmark/DecodeFileBenchmark.cpp:9
7: ...............................................................................
7: 
7: benchmark name                       samples       iterations    estimated
7:                                      mean          low mean      high mean
7:                                      std dev       low std dev   high std dev
7: -------------------------------------------------------------------------------
7: Decoding "open_suse_15_3.torrent"              100             1     3.3174 ms 
7:                                         33.2376 us    33.1525 us    33.4895 us 
7:                                          681.59 ns     277.04 ns    1.46772 us                                                                             
7: 
7: -------------------------------------------------------------------------------
```

Catch2 is also required for the benchmarking. Make sure that you're running benchmarks for the release build.

# Add to project

Clone as a submodule. Then add like this:

```cmake
add_subdirectory(bencode)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/bencode/include)
```

Link your target(s) against it:
```cmake
target_link_libraries(my_target PRIVATE bencode)
```
