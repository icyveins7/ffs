#include "ffs.h"
#include <vector>
#include <cmath>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#define SINGLE_REL_THRESHOLD_SHORT 1e-6 // for length 1e5
#define SINGLE_REL_THRESHOLD_LONG 1e-5 // for length 1e8

template <typename T>
void test_basic(size_t len, double freq, double phase, double threshold)
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
    bool printed = false;
    for (size_t i = 0; i < data.size(); i++)
    {
        // we always cast to double first to calculate the checks
        std::complex<double> correct = static_cast<std::complex<double>>(data2[i]) * std::complex<double>(
            std::cos(2 * M_PI * freq * i + phase),
            std::sin(2 * M_PI * freq * i + phase)
        );

        double dreal = static_cast<double>(data[i].real());
        double dimag = static_cast<double>(data[i].imag());

        if (std::abs(dreal - correct.real()) / correct.real() > threshold && !printed)
        {
            printf("real[%zd]: %f(computed) vs %f(check)\n", i, dreal, correct.real());
            printed = true; // just show once
        }

        REQUIRE_THAT(
            dreal, 
            Catch::Matchers::WithinRel(correct.real(), threshold));
        REQUIRE_THAT(
            dimag,
            Catch::Matchers::WithinRel(correct.imag(), threshold));
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
    SECTION("len 1e5, freq 1e-9, phase 0.1"){
        test_basic<float>(100000, 1e-9, 0.1, SINGLE_REL_THRESHOLD_SHORT);
    }

    // This is to check non-multiple of UNROLL lengths
    SECTION("len 1e5-1, freq 1e-9, phase 0.1"){
        test_basic<float>(99999, 1e-9, 0.1, SINGLE_REL_THRESHOLD_SHORT);
    }
    
    // For long lengths, relative threshold must be increased in order to pass
    SECTION("len 1e8, freq 1e-9, phase 0.1"){
        test_basic<float>(100000000, 1e-9, 0.1, SINGLE_REL_THRESHOLD_LONG);
    }

}


/*
//////////////////////////////////////////////////////////////////////////////////////////
BENCHMARKS
//////////////////////////////////////////////////////////////////////////////////////////
*/

template <typename T>
void benchmark_basic(std::vector<std::complex<T>>& data)
{
    ffs::shiftVector<T>(data, 1e-9, 0.1);
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
        constexpr size_t len = 100000;
        std::vector<std::complex<double>> data(len);
        for (int i = 0; i < data.size(); i++)
            data[i] = std::complex<double>(i+1, i+1);

        BENCHMARK("ffs")
        {
            return benchmark_basic<double>(data);
        };

        BENCHMARK("naive")
        {
            return benchmark_naive<double>(data);
        };
    }

    SECTION("len 1e6")
    {
        constexpr size_t len = 1000000;
        std::vector<std::complex<double>> data(len);
        for (int i = 0; i < data.size(); i++)
            data[i] = std::complex<double>(i+1, i+1);

        BENCHMARK("ffs")
        {
            return benchmark_basic<double>(data);
        };

        BENCHMARK("naive")
        {
            return benchmark_naive<double>(data);
        };
    }
    
}

TEST_CASE("benchmark float", "[benchmark],[float]")
{
    SECTION("len 1e5")
    {
        constexpr size_t len = 100000;
        std::vector<std::complex<float>> data(len);
        for (int i = 0; i < data.size(); i++)
            data[i] = std::complex<float>(i+1, i+1);

        BENCHMARK("ffs")
        {
            return benchmark_basic<float>(data);
        };

        BENCHMARK("naive")
        {
            return benchmark_naive<float>(data);
        };
    }
    
    SECTION("len 1e6")
    {
        constexpr size_t len = 1000000;
        std::vector<std::complex<float>> data(len);
        for (int i = 0; i < data.size(); i++)
            data[i] = std::complex<float>(i+1, i+1);

        BENCHMARK("ffs")
        {
            return benchmark_basic<float>(data);
        };

        BENCHMARK("naive")
        {
            return benchmark_naive<float>(data);
        };
    }
    
}

