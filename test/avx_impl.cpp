#include "ffs_avx_impl.h"

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

    SECTION("std::complex array[4]")
    {
        std::complex<double> x[4] = {
            std::complex<double>(-4.0, -3.0),
            std::complex<double>(-2.0, -1.0),
            std::complex<double>(0.0, 1.0),
            std::complex<double>(2.0, 3.0)
        };
        std::complex<float> y[4];

        doubleToFloatIntrinsic8(reinterpret_cast<double*>(x),reinterpret_cast<float*>(y));

        for (int i = 0; i < 4; i++)
        {
            REQUIRE(y[i].real() == static_cast<float>(x[i].real()));
            REQUIRE(y[i].imag() == static_cast<float>(x[i].imag()));
        }
    }
}



////////////////////////////////////////////////////////////
TEST_CASE("complex multiply", "[avx], [multiply], [vecXvec]")
{
    SECTION("float 4x4"){
        std::complex<float> x[4];
        std::complex<float> y[4];
        std::complex<float> check[4];

        // Set some values
        for (int i = 0; i < 4; i++)
        {
            x[i] = std::complex<float>(i+1, i+2);
            y[i] = std::complex<float>(i+4, i+3);
            check[i] = y[i];
        }

        // Run the intrinsic
        complexMulIntrinsic_4x4_32fc(x, y);

        // Check
        for (int i = 0; i < 4; i++)
        {
            std::complex<float> z = check[i] * x[i];
            REQUIRE(y[i].real() == z.real());
            REQUIRE(y[i].imag() == z.imag());
        }
    }

    SECTION("double 2x2"){
        std::complex<double> x[2];
        std::complex<double> y[2];
        std::complex<double> check[2];
        
        // Set some values
        for (int i = 0; i < 2; i++)
        {
            x[i] = std::complex<double>(i+1, i+2);
            y[i] = std::complex<double>(i+4, i+3);
            check[i] = y[i];
        }
       
        // Run the intrinsic
        complexMulIntrinsic_2x2_64fc(x, y);

        // Check
        for (int i = 0; i < 2; i++)
        {
            std::complex<double> z = check[i] * x[i];
            REQUIRE(y[i].real() == z.real());
            REQUIRE(y[i].imag() == z.imag());
        }
    }
}


////////////////////////////////////////////////////////////

TEST_CASE("complex multiply 2xScalar", "[avx], [multiply], [2xScalar]")
{
    // Only have a double version for this
    SECTION("double")
    {
        std::complex<double> x[2] = {
            std::complex<double>(1.0, 2.0),
            std::complex<double>(5.0, 6.0)
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


