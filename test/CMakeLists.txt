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

# Add our include dir
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/)

# Define test executable for generic implementation
add_executable(basic_generic basic.cpp)
target_link_libraries(basic_generic PUBLIC Catch2::Catch2WithMain)


# Define test executables for AVX implementation
add_executable(avx_impl avx_impl.cpp)
target_link_libraries(avx_impl PUBLIC Catch2::Catch2WithMain)
if (MSVC)
    target_compile_options(avx_impl PUBLIC /arch:AVX /Fa /FA)
endif()

add_executable(basic_avx basic.cpp)
target_link_libraries(basic_avx PUBLIC Catch2::Catch2WithMain)
if (MSVC)
    target_compile_options(basic_avx PUBLIC /arch:AVX /Fa /FA)
endif()



include(CTest)
include(Catch)
catch_discover_tests(basic_generic)
catch_discover_tests(avx_impl)
catch_discover_tests(basic_avx)