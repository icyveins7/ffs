#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <vector>

namespace ffs
{
    /// @brief Shift an input complex array by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample. Defaults to 1.
    /// @tparam UNROLL Optimization parameter.
    /// @param array Input complex array. Will be overwritten with the shifted values.
    /// @param size Length of the input array.
    /// @param freq Normalized frequency i.e. [0, 1)
    /// @param startPhase Start phase of the frequency shift in radians.
    template <typename T, size_t UNROLL = 1>
    void shiftArray(
        std::complex<T> *array,
        const size_t size,
        const T freq,
        const T startPhase
    ){
        // Perform some checks
        if (freq < 0 || freq >= 1)
            throw std::invalid_argument("freq must be in [0, 1)");

        // Allocate array for initial tones
        std::complex<T> tones[UNROLL];
        // Initialize them
        for (size_t i = 0; i < UNROLL; ++i)
            tones[i] = std::complex<T>(std::cos(startPhase + i*2*M_PI*freq), std::sin(startPhase + i*2*M_PI*freq));
        
        // Compute the step
        std::complex<T> step(std::cos(2*M_PI*freq*UNROLL), std::sin(2*M_PI*freq*UNROLL));

        // Main loop
        for (size_t i = 0; i < size; i += UNROLL)
        {
            /* 
            Note that this is technically a 'soft' unroll as we expect the compiler 
            to unroll this given that UNROLL is a template and/or constexpr parameter.
            */
            for (size_t j = 0; j < UNROLL; ++j)
            {
                if (i != 0) // don't shift the tone on the first iteration
                    tones[j] *= step;

                if (i + j < size) // then we multiply it into the array
                    array[i + j] *= tones[j];
            }
        }
    };


    /// @brief Shift an input complex vector by a normalized frequency and start phase.
    /// @tparam T Data type of real/imag sample.
    /// @tparam UNROLL Optimization parameter. Defaults to 1.
    /// @param vec Input complex vector. Will be overwritten with the shifted values.
    /// @param freq Normalized frequency i.e. [0, 1)
    /// @param startPhase Start phase of the frequency shift in radians.
    template <typename T, size_t UNROLL = 1>
    void shiftVector(
        std::vector<std::complex<T>> &vec,
        const T freq,
        const T startPhase
    ){
        // Call the shiftArray function
        shiftArray<T, UNROLL>(vec.data(), vec.size(), freq, startPhase);
    };

}
