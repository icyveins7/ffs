#include "ffs.h"
#include <vector>
#include <complex>
#include <iostream>

#ifdef __APPLE__
using namespace ffsh;
#else
using namespace ffs;
#endif


int main()
{
    std::vector<std::complex<float>> v(4);
    for (int i = 0; i < v.size(); ++i)
        v[i] = std::complex<float>(i, i);


    shiftVector(v, 0.2f, 0.0f);

    for (auto& i : v)
        printf("%f %f\n", i.real(), i.imag());

    return 0;
}