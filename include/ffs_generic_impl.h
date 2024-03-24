#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <vector>

#ifdef __APPLE__
namespace ffsh
#else
namespace ffs
#endif
{
    /// @brief Shift an input complex array by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample.
    /// @tparam double Data type for the tone and step real/imag values.
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
    ){
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
            // Explicitly unroll
            array[i+0] *= tones[0];
            array[i+1] *= tones[1];
            array[i+2] *= tones[2];
            array[i+3] *= tones[3];

            // Adjust tones
            tones[0] *= step;
            tones[1] *= step;
            tones[2] *= step;
            tones[3] *= step;
        }
       
        // Remainder loop
        for (size_t i = 0; i < size % 4; ++i)
        {
            array[size-size%4 + i] *= tones[i];
            tones[i] *= step;
        }
    };


    /// @brief Shift an input complex vector by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample.
    /// @tparam double Data type for the tone and step real/imag values.
    /// @tparam doubleNROLL Optimization parameter. Defaults to 1.
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
