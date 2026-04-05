cmake_minimum_required(VERSION 3.20)
project(matrix)

option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

find_package(OpenCL REQUIRED)
add_executable(matrix)
target_compile_features(matrix PUBLIC cxx_std_23)
target_link_libraries(matrix PRIVATE OpenCL::OpenCL)

target_include_directories(matrix PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_sources(matrix PRIVATE
    src/main.cpp
)

target_compile_definitions(matrix PRIVATE
    KERNELS_ABS_PATH="${CMAKE_SOURCE_DIR}"
    CL_TARGET_OPENCL_VERSION=200
    CL_HPP_TARGET_OPENCL_VERSION=200
)

target_compile_options(matrix PRIVATE
    -march=native
    -mfma

    $<$<CONFIG:Release>:
        -O3
        -ffast-math
        -funroll-loops
    >

    $<$<CONFIG:Debug>:
        -O0
        -g
        -Wall
        -Wextra
        -Wpedantic
        -DDEBUG
    >
)

if(ENABLE_ASAN)
    target_compile_options(matrix PRIVATE $<$<CONFIG:Debug>: -fsanitize=address>)
    target_link_options(matrix PRIVATE $<$<CONFIG:Debug>: -fsanitize=address>)
endif()