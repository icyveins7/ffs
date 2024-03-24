#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <vector>
#include <immintrin.h>

#ifdef _MSC_VER // for MSVC
#define RESTRICT __restrict
#else // For GCC / clang
#define RESTRICT __restrict__
#endif

#ifdef __APPLE__
namespace ffsh
#else
namespace ffs
#endif
{
    /*
    As of 23 Feb 2024, MSVC up to 19.38 does not auto vectorise std::complex with AVX properly.
    It also does not auto vectorise the double to float casts using AVX properly.
    Basically MSVC just generally sucks at autovectorising, regardless of what we do.
    Hence we have to handroll the intrinsics.
    This is generally done by just copying and reverse-translating the assembly output
    from GCC's compilation with -O3 -ffast-math -mavx. 
    */


    static inline void doubleToFloatIntrinsic8(
        const double * RESTRICT x, float * RESTRICT y
    ){
        __m256d ymm0 = _mm256_loadu_pd(&x[0]);
        __m256d ymm1 = _mm256_loadu_pd(&x[4]);

        __m128 xmm0 = _mm256_cvtpd_ps(ymm0);
        __m128 xmm1 = _mm256_cvtpd_ps(ymm1);

        __m256 ymm2 = _mm256_set_m128(xmm1, xmm0);

        _mm256_storeu_ps(y, ymm2);
    }

    static inline void complexMulIntrinsic_4x4_32fc(
        const std::complex<float> * RESTRICT x,
        std::complex<float> * RESTRICT z
    ){
        __m256 ymm0 = _mm256_loadu_ps(reinterpret_cast<const float*>(x));
        __m256 ymm3 = _mm256_loadu_ps(reinterpret_cast<const float*>(z));

        __m256 ymm1 = _mm256_permute_ps(ymm3, 160);
        __m256 ymm2 = _mm256_permute_ps(ymm3, 245);
        
        ymm1 = _mm256_mul_ps(ymm0,ymm1);
        
        ymm0 = _mm256_permute_ps(ymm0, 177);
        ymm0 = _mm256_mul_ps(ymm0,ymm2);

        ymm1 = _mm256_addsub_ps(ymm1,ymm0);

        _mm256_storeu_ps(reinterpret_cast<float*>(z), ymm1);
    }

    static inline void complexMulIntrinsic_2x2_64fc(
        const std::complex<double> * RESTRICT x,
        std::complex<double> * RESTRICT z
    ){
        __m256d ymm2 = _mm256_loadu_pd(reinterpret_cast<const double*>(x));
        __m256d ymm3 = _mm256_loadu_pd(reinterpret_cast<const double*>(z));

        __m256d ymm1 = _mm256_permute_pd(ymm3, 0);
        __m256d ymm0 = _mm256_permute_pd(ymm3, 15);

        ymm1 = _mm256_mul_pd(ymm1, ymm2);

        ymm2 = _mm256_permute_pd(ymm2, 5);
        ymm0 = _mm256_mul_pd(ymm0, ymm2);

        ymm1 = _mm256_addsub_pd(ymm1,ymm0);

        _mm256_storeu_pd(reinterpret_cast<double*>(z), ymm1);
    }

    /// @brief Performs z[i] *= x for i = 0,1
    /// @param x Constant to multiply into z
    /// @param z Input/output array vector. 
    static inline void complexMulIntrinsic_2xScalar(
        const std::complex<double> &x,
        std::complex<double> * RESTRICT z
    ){
        // Broadcast the constant into both 128 bit lanes
        __m256d ymm2 = _mm256_broadcast_pd(reinterpret_cast<const __m128d*>(&x));
        // Load 2 complex doubles
        __m256d ymm3 = _mm256_loadu_pd(reinterpret_cast<const double*>(z));

        // Perform the complex multiplies
        __m256d ymm1 = _mm256_permute_pd(ymm3, 0);
        __m256d ymm0 = _mm256_permute_pd(ymm3, 15);
        ymm1 = _mm256_mul_pd(ymm1, ymm2);

        ymm2 = _mm256_permute_pd(ymm2, 5);
        ymm0 = _mm256_mul_pd(ymm0, ymm2);

        ymm1 = _mm256_addsub_pd(ymm1,ymm0);

        _mm256_storeu_pd(reinterpret_cast<double*>(z), ymm1);
    }



    /// @brief Shift an input complex array by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample.
    /// @tparam 4 Optimization parameter. Defaults to 1.
    /// @param array Input complex array. Will be overwritten with the shifted values.
    /// @param size Length of the input array.
    /// @param freq Normalized frequency i.e. [0, 1)
    /// @param startPhase Start phase of the frequency shift in radians.
    template <typename T>
    void shiftArray(
        std::complex<T> *array,
        const size_t size,
        const double freq,
        const double startPhase
    );
   
    
    template <>
    inline void shiftArray(
        std::complex<float> *array,
        const size_t size,
        const double freq,
        const double startPhase
    ){
        // Allocate array for initial tones
        std::complex<double> tones[4];
        std::complex<float> tones_32f[4];
        // Initialize them
        for (size_t i = 0; i < 4; ++i)
            tones[i] = std::complex<double>(std::cos(startPhase + i*2*M_PI*freq), std::sin(startPhase + i*2*M_PI*freq));
        
        // Compute the step
        std::complex<double> step(std::cos(2*M_PI*freq*4), std::sin(2*M_PI*freq*4));


        // Main loop
        for (size_t i = 0; i < size-size%4; i += 4)
        {
            // Cast the tones to float
            doubleToFloatIntrinsic8(
                reinterpret_cast<const double*>(tones),
                reinterpret_cast<float*>(tones_32f)
            );

            // Multiply into array
            complexMulIntrinsic_4x4_32fc(
                tones_32f,
                &array[i]
            );

            // Increment tones
            complexMulIntrinsic_2xScalar(step, tones);
        }

        // Remainder loop
        for (size_t i = 0; i < size % 4; ++i)
        {
            tones_32f[i] = static_cast<std::complex<float>>(tones[i]);
            array[size-size%4 + i] = array[size-size%4 + i] * tones_32f[i];
        }
    };

    template <>
    inline void shiftArray(
        std::complex<double> *array,
        const size_t size,
        const double freq,
        const double startPhase
    ){
        // We don't need the second array of tones for this, but
        // we will keep it at 4 tones for consistency even though
        // for doubles, we can only fit 2 tones in the AVX registers.

        // Allocate array for initial tones
        std::complex<double> tones[4];
        // Initialize them
        for (size_t i = 0; i < 4; ++i)
            tones[i] = std::complex<double>(std::cos(startPhase + i*2*M_PI*freq), std::sin(startPhase + i*2*M_PI*freq));
        
        // Compute the step
        std::complex<double> step(std::cos(2*M_PI*freq*4), std::sin(2*M_PI*freq*4));

        // Main loop
        for (size_t i = 0; i < size-size%4; i += 4)
        {
            // Multiply into array
            complexMulIntrinsic_2x2_64fc(
                &tones[0],
                &array[i+0]
            );
            complexMulIntrinsic_2x2_64fc(
                &tones[2],
                &array[i+2]
            );

            // Increment tones
            complexMulIntrinsic_2xScalar(step, &tones[0]);
            complexMulIntrinsic_2xScalar(step, &tones[2]);
        }

        // Remaining loops
        for (size_t i = 0; i < size % 4; ++i)
        {
            array[size-size%4 + i] = array[size-size%4 + i] * tones[i];
        }
    }


    /// @brief Shift an input complex vector by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample.
    /// @tparam U Data type for the tone and step real/imag values.
    /// @tparam 4 Optimization parameter. Defaults to 1.
    /// @param vec Input complex vector. Will be overwritten with the shifted values.
    /// @param freq Normalized frequency i.e. [0, 1)
    /// @param startPhase Start phase of the frequency shift in radians.
    template <typename T>
    void shiftVector(
        std::vector<std::complex<T>> &vec,
        const double freq,
        const double startPhase
    ){
        // Call the shiftArray function
        shiftArray<T>(vec.data(), vec.size(), freq, startPhase);
    };

}


