#include "ffs_avx_impl.h"
#include <vector>
#include <cmath>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#ifdef __APPLE__
using namespace ffsh;
#else
using namespace ffs;
#endif


TEST_CASE("double to float cast", "[avx], [cast]")
{
    SECTION("real array[8]")
    {
        double x[8] = {-4.0, -3.0, -2.0, -1.0, 0.0, 1.0, 2.0, 3.0};
        float y[8];

        doubleToFloatIntrinsic8(x, y);

        for (int i = 0; i < 8; i++)
        {
            REQUIRE(y[i] == static_cast<float>(x[i]));
        }
    }
}



////////////////////////////////////////////////////////////

template <typename T>
void test_avx_complexMul()
{
    std::complex<T> x[4];
    std::complex<T> y[4];
    std::complex<T> check[4];

    // Set some values
    for (int i = 0; i < 4; i++)
    {
        x[i] = std::complex<T>(i+1, i+1);
        y[i] = std::complex<T>(i+1, i+1);
        check[i] = y[i];
    }

    // Run the intrinsic
    complexMulIntrinsic_4x4<T>(x, y);

    // Check
    for (int i = 0; i < 4; i++)
    {
        std::complex<T> z = check[i] * x[i];
        REQUIRE(y[i].real() == z.real());
        REQUIRE(y[i].imag() == z.imag());
    }
}

TEST_CASE("complex multiply 4x4", "[avx], [multiply], [4x4]")
{
    SECTION("float"){
        test_avx_complexMul<float>();
    }    
}


////////////////////////////////////////////////////////////

TEST_CASE("complex multiply 2xScalar", "[avx], [multiply], [2xScalar]")
{
    // Only have a double version for this
    SECTION("double")
    {
        std::complex<double> x[2] = {
            std::complex<double>(1.0, 1.0),
            std::complex<double>(2.0, 2.0)
        };

        std::complex<double> check[2];
        for (int i = 0; i < 2; i++){
            check[i] = x[i];
        }

        std::complex<double> scalar(3.0, 4.0);

        // Run the intrinsic
        complexMulIntrinsic_2xScalar(scalar, x);

        // Check
        for (int i = 0; i < 2; i++)
        {
            std::complex<double> z = check[i] * scalar;
            REQUIRE(x[i].real() == z.real());
            REQUIRE(x[i].imag() == z.imag());
        }   
    }
}


