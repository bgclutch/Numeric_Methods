cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 23)

project(crit_error)
add_executable(crit_error)

option(ENABLE_ASAN OFF)

target_compile_features(crit_error PUBLIC cxx_std_23)

target_include_directories(crit_error PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_sources(crit_error PRIVATE
    src/main.cpp
)

target_compile_options(crit_error PRIVATE
$<$<CONFIG:Debug>:
    -Wall
    -Wextra
    -Wpedantic
    -DDEBUG
    -Wmissing-declarations
    -Wcast-align
    -Wshadow
    -Wsign-conversion>
)

if(ENABLE_ASAN)
    target_compile_options(crit_error PRIVATE $<$<CONFIG:Debug>: -fsanitize=address>)
    target_link_options(crit_error PRIVATE $<$<CONFIG:Debug>: -fsanitize=address>)
endif()