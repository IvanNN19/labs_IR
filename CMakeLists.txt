cmake_minimum_required(VERSION 3.10)
project(SearchEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

add_library(common STATIC ${SRC_DIR}/common/utils.cpp)
target_include_directories(common PUBLIC ${SRC_DIR})

add_executable(crawler
    ${SRC_DIR}/crawler/crawler.cpp
    ${SRC_DIR}/crawler/main.cpp
)
target_link_libraries(crawler common)

add_executable(tokenizer
    ${SRC_DIR}/tokenizer/tokenizer.cpp
    ${SRC_DIR}/tokenizer/main.cpp
)
target_link_libraries(tokenizer common)

add_executable(stemmer
    ${SRC_DIR}/stemmer/stemmer.cpp
    ${SRC_DIR}/stemmer/main.cpp
)
target_link_libraries(stemmer common)

add_executable(zipf_analyzer
    ${SRC_DIR}/zipf/zipf_analyzer.cpp
    ${SRC_DIR}/zipf/main.cpp
)
target_link_libraries(zipf_analyzer common)

add_executable(build_index
    ${SRC_DIR}/index/inverted_index.cpp
    ${SRC_DIR}/index/main.cpp
)
target_link_libraries(build_index common)

add_executable(bool_search
    ${SRC_DIR}/index/inverted_index.cpp
    ${SRC_DIR}/search/bool_search.cpp
    ${SRC_DIR}/search/main.cpp
)
target_link_libraries(bool_search common)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(test_tokenizer 
        ${SRC_DIR}/tokenizer/tokenizer.cpp
        tests/test_tokenizer.cpp
    )
    target_link_libraries(test_tokenizer common)
    
    add_executable(test_stemmer
        ${SRC_DIR}/stemmer/stemmer.cpp
        tests/test_stemmer.cpp
    )
    target_link_libraries(test_stemmer common)
endif()
