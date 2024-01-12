# ffs

$$
x[n] \rightarrow x[n] e^{i 2 \pi f n + \phi}
$$

Fast frequency shifting of complex samples. Inspired by the discussion [here](https://stackoverflow.com/questions/51735576/fast-and-accurate-iterative-generation-of-sine-and-cosine-for-equally-spaced-ang).

This implementation allows you to be faster than some other libraries, which generate the frequency shift tone in a separate array, and then require you to manually multiply the tone into your samples; here you generate them and multiply them in on-the-fly, saving you the potentially large memory cost of the tone's array and also some runtime overhead.

The current implementation accumulates some error over long arrays.

## Usage

Include the header `ffs.h` and simply call either of the two templated functions, which are namespaced:

1. `ffs::shiftArray` for C arrays.
2. `ffs::shiftVector` for C++ vectors.

## Tests & Benchmarks

Catch2 is used for testing. See the `CMakeLists.txt` for details.

### Ryzen 5 5600X, Windows 10, MSVC 2022 (Release Configuration)

```
benchmark name                       samples       iterations    estimated
                                     mean          low mean      high mean
                                     std dev       low std dev   high std dev
-------------------------------------------------------------------------------
ffs, UNROLL 1                                  100             1    19.1637 ms
                                        191.532 us    191.302 us    191.887 us
                                        1.43363 us    1.03681 us    1.98936 us

ffs, UNROLL 4                                  100             1     9.0314 ms
                                         89.794 us     89.567 us      90.25 us
                                         1.5815 us    943.862 ns    2.83677 us

naive                                          100             1    48.0757 ms
                                         478.08 us    477.629 us    478.865 us
                                        2.95508 us    1.97238 us    4.87525 us
```