# ffs

$$
x[n] \rightarrow x[n] e^{i 2 \pi f n + \phi}
$$

Fast frequency shifting of complex samples. Inspired by the discussion [here](https://stackoverflow.com/questions/51735576/fast-and-accurate-iterative-generation-of-sine-and-cosine-for-equally-spaced-ang).

This implementation allows you to be faster than some other libraries, which generate the frequency shift tone in a separate array, and then require you to manually multiply the tone into your samples; here you generate them and multiply them in on-the-fly, saving you the potentially large memory cost of the tone's array and also some runtime overhead.

The current implementation accumulates some error over very long arrays; this is by design, as the frequency shift exponential is incremented step-wise over every sample, so this accumulates a non-negligible error after some time. This is suppressed by always performing the computations at `double` precision. If the error is determined to be too large, the computation can be split into multiple function call batches, as the first sample frequency shift is computed as per normal.

However, doing it this way does incur extra cost when shifting single-precision `float` arrays, as the input is internally cast to `double`s for the frequency shift and then cast back for storage.

## Usage

Include the header `ffs.h` and simply call either of the two templated functions, which are namespaced:

1. `ffs::shiftArray` for C arrays.
2. `ffs::shiftVector` for C++ vectors.

### MacOS

For Macs, the namespace `ffs` conflicts with some other in-built namespace, so I've renamed it to `ffsh`.

## Tests & Benchmarks

Catch2 is used for testing. See the `CMakeLists.txt` for details.

### Ryzen 5 5600X, Windows 10, MSVC 2022 (Release Configuration with AVX instructions)

```
-------------------------------------------------------------------------------
benchmark double
  len 1e5
-------------------------------------------------------------------------------
E:\gitrepos\ffs\test\basic.cpp(122)
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
ffs                                            100             1     6.5461 ms
                                         65.817 us     65.225 us     68.079 us
                                        5.15526 us    1.15087 us    11.9096 us

naive                                          100             1      46.23 ms
                                        458.047 us    457.514 us    458.842 us
                                        3.26806 us    2.39509 us    4.96366 us


-------------------------------------------------------------------------------
benchmark double
  len 1e6
-------------------------------------------------------------------------------
E:\gitrepos\ffs\test\basic.cpp(140)
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
ffs                                            100             1    65.4139 ms
                                        658.554 us      653.8 us    674.475 us
                                        38.5702 us    6.26944 us    85.1791 us

naive                                          100             1     462.71 ms
                                        4.60644 ms     4.6033 ms     4.6094 ms
                                        15.5268 us    13.1442 us    18.8297 us


-------------------------------------------------------------------------------
benchmark float
  len 1e5
-------------------------------------------------------------------------------
E:\gitrepos\ffs\test\basic.cpp(162)
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
ffs                                            100             1     7.2411 ms
                                          72.45 us     72.314 us     72.655 us
                                         837.34 ns     608.45 ns    1.11194 us

naive                                          100             1    48.3568 ms
                                        479.566 us    478.986 us    480.476 us
                                        3.63049 us    2.55699 us    6.21933 us


-------------------------------------------------------------------------------
benchmark float
  len 1e6
-------------------------------------------------------------------------------
E:\gitrepos\ffs\test\basic.cpp(180)
...............................................................................

benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
ffs                                            100             1    72.5538 ms
                                        725.744 us    724.458 us    728.088 us
                                         8.6057 us    5.38523 us    13.2087 us

naive                                          100             1    483.083 ms
                                        4.81551 ms    4.81002 ms    4.83373 ms
                                        46.0573 us    15.5602 us    104.114 us


```
