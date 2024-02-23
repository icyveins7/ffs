cmake_minimum_required(VERSION 3.20)

project(ffs_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)

# Check for user-specified CMAKE_PREFIX_PATH, otherwise generate it from VCPKG_DIR environment variable
if (NOT DEFINED CMAKE_PREFIX_PATH)
    message("CMAKE_PREFIX_PATH not defined. Searching for env var VCPKG_DIR..")
    if (DEFINED ENV{VCPKG_DIR})
        message("VCPKG_DIR is " $ENV{VCPKG_DIR})
        # Set CMAKE_PREFIX_PATH to the VCPKG_DIR
        if (WIN32)
            if ((CMAKE_SIZEOF_VOID_P GREATER 4))
                set(CMAKE_PREFIX_PATH $ENV{VCPKG_DIR}\\installed\\x64-windows\\share) # This is the place to find the packages usually
            else()
                set(CMAKE_PREFIX_PATH $ENV{VCPKG_DIR}\\installed\\x86-windows\\share)
            endif()
        endif()
        message("CMAKE_PREFIX_PATH is " ${CMAKE_PREFIX_PATH})
    else()
        message("VCPKG_DIR not found.")
    endif()
else()
    message("CMAKE_PREFIX_PATH specified by user: " ${CMAKE_PREFIX_PATH})
endif()

# Add Catch2 library
find_package(Catch2 3 REQUIRED)
include_directories(${Catch2_INCLUDE_DIRS})

# Use bigger instruction sets
if (MSVC)
    message("For MSVC, using /arch:AVX")
    add_compile_options(/arch:AVX /Fa /FA)
endif()

# Add our include dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

# Define test executable for vectors
add_executable(basic basic.cpp)
target_link_libraries(basic PUBLIC Catch2::Catch2WithMain)

add_executable(avx_impl avx_impl.cpp)
target_link_libraries(avx_impl PUBLIC Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(basic)
catch_discover_tests(avx_impl)