#include "ffs.h"
#include <vector>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

template <typename T>
void test_basic(size_t len, T freq, T phase, double threshold)
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
    ffs::shiftVector<T, 4>(data, freq, phase);

    // Check
    for (size_t i = 0; i < data.size(); i++)
    {
        std::complex<T> correct = data2[i] * std::complex<T>(
            std::cos(2 * M_PI * freq * i + phase),
            std::sin(2 * M_PI * freq * i + phase)
        );

        REQUIRE(std::abs((data[i].real() - correct.real()) / correct.real()) < threshold);
        REQUIRE(std::abs((data[i].imag() - correct.imag()) / correct.imag()) < threshold);
    }
}

TEST_CASE("basic double", "[basic],[double]")
{
    SECTION("len 1e5, freq 1e-9, phase 0.1, relative threshold 1e-9"){
        test_basic<double>(100000, 1e-9, 0.1, 1e-9);
    }

    SECTION("len 1e8, freq 1e-9, phase 0.1, relative threshold 1e-7"){
        test_basic<double>(100000000, 1e-9, 0.1, 1e-7);
    }

}


/*
//////////////////////////////////////////////////////////////////////////////////////////
BENCHMARKS
//////////////////////////////////////////////////////////////////////////////////////////
*/

template <typename T, size_t UNROLL>
void benchmark_basic(std::vector<std::complex<T>>& data)
{
    ffs::shiftVector<T, UNROLL>(data, 1e-9, 0.1);
}

template <typename T>
void benchmark_naive(std::vector<std::complex<T>>& data)
{
    for (size_t i = 0; i < data.size(); i++)
    {
        data[i] = data[i] * std::complex<T>(
            std::cos(2 * M_PI * 1e-9 * i + 0.1),
            std::sin(2 * M_PI * 1e-9 * i + 0.1)
        );
    }
}

TEST_CASE("benchmark double", "[benchmark],[double]")
{
    SECTION("len 1e5")
    {
        std::vector<std::complex<double>> data(100000);
        for (int i = 0; i < data.size(); i++)
            data[i] = std::complex<double>(i+1, i+1);

        BENCHMARK("ffs, UNROLL 1")
        {
            return benchmark_basic<double, 1>(data);
        };

        BENCHMARK("ffs, UNROLL 4")
        {
            return benchmark_basic<double, 4>(data);
        };

        BENCHMARK("naive")
        {
            return benchmark_naive<double>(data);
        };
    }
    
}
