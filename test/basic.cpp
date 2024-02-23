#include "ffs.h"
#include <vector>
#include <cmath>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>


template <typename T>
void test_basic(size_t len, double freq, double phase, T threshold)
{
    // Create some vectors
    std::vector<std::complex<T>> data(len);
    std::vector<std::complex<T>> data2(len);

    // Write some values to it
    for (size_t i = 0; i < data.size(); i++)
    {
        data[i] = std::complex<T>(i+1, i+1);
        data2[i] = data[i];
    }

    // Shift the data with our function
    ffs::shiftVector<T>(data, freq, phase);

    // Check
    for (size_t i = 0; i < data.size(); i++)
    {
        std::complex<T> correct = data2[i] * std::complex<T>(
            std::cos(2 * M_PI * freq * i + phase),
            std::sin(2 * M_PI * freq * i + phase)
        );

        REQUIRE_THAT(data[i].real(), Catch::Matchers::WithinRel(correct.real(), threshold));
        REQUIRE_THAT(data[i].imag(), Catch::Matchers::WithinRel(correct.imag(), threshold));
    }
}


// For threshold checks, we are okay as long as it's within relative 1e-9
TEST_CASE("basic double", "[basic],[double]")
{
    SECTION("len 1e5, freq 1e-9, phase 0.1, relative threshold 1e-9"){
        test_basic<double>(100000, 1e-9, 0.1, 1e-9);
    }

    // This is to check non-multiple of UNROLL lengths
    SECTION("len 1e5-1, freq 1e-9, phase 0.1, relative threshold 1e-9"){
        test_basic<double>(99999, 1e-9, 0.1, 1e-9);
    }

    // For long lengths, relative threshold must be increased in order to pass
    SECTION("len 1e8, freq 1e-9, phase 0.1, relative threshold 1e-7"){
        test_basic<double>(100000000, 1e-9, 0.1, 1e-7);
    }

}

TEST_CASE("basic float", "[basic],[float]")
{
    SECTION("len 1e5, freq 1e-9, phase 0.1, relative threshold 1e-8"){
        test_basic<float>(100000, 1e-9, 0.1, 1e-9);
    }

    // This is to check non-multiple of UNROLL lengths
    SECTION("len 1e5-1, freq 1e-9, phase 0.1, relative threshold 1e-8"){
        test_basic<float>(99999, 1e-9, 0.1, 1e-9);
    }
    
    // For long lengths, relative threshold must be increased in order to pass
    SECTION("len 1e8, freq 1e-9, phase 0.1, relative threshold 1e-5"){
        test_basic<float>(100000000, 1e-9, 0.1, 1e-5);
    }

}


/*
//////////////////////////////////////////////////////////////////////////////////////////
BENCHMARKS
//////////////////////////////////////////////////////////////////////////////////////////
*/

// template <typename T, typename U, size_t UNROLL>
// void benchmark_basic(std::vector<std::complex<T>>& data)
// {
//     ffs::shiftVector<T, U, UNROLL>(data, 1e-9, 0.1);
// }

// template <typename T>
// void benchmark_naive(std::vector<std::complex<T>>& data)
// {
//     for (size_t i = 0; i < data.size(); i++)
//     {
//         data[i] = data[i] * std::complex<T>(
//             std::cos(2 * M_PI * 1e-9 * i + 0.1),
//             std::sin(2 * M_PI * 1e-9 * i + 0.1)
//         );
//     }
// }

// TEST_CASE("benchmark double", "[benchmark],[double]")
// {
//     SECTION("len 1e5")
//     {
//         std::vector<std::complex<double>> data(100000);
//         for (int i = 0; i < data.size(); i++)
//             data[i] = std::complex<double>(i+1, i+1);

//         BENCHMARK("ffs, UNROLL 1")
//         {
//             return benchmark_basic<double, double, 1>(data);
//         };

//         BENCHMARK("ffs, UNROLL 4")
//         {
//             return benchmark_basic<double, double, 4>(data);
//         };

//         BENCHMARK("naive")
//         {
//             return benchmark_naive<double>(data);
//         };
//     }
    
// }

// TEST_CASE("benchmark float", "[benchmark],[float]")
// {
//     SECTION("len 1e5")
//     {
//         std::vector<std::complex<float>> data(100000);
//         for (int i = 0; i < data.size(); i++)
//             data[i] = std::complex<float>(i+1, i+1);

//         BENCHMARK("ffs, UNROLL 1")
//         {
//             return benchmark_basic<float, double, 1>(data);
//         };

//         BENCHMARK("ffs, UNROLL 4")
//         {
//             return benchmark_basic<float, double, 4>(data);
//         };

//         BENCHMARK("naive")
//         {
//             return benchmark_naive<float>(data);
//         };
//     }
    
// }

