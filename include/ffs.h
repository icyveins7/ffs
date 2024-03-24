#pragma once

#ifdef __AVX__
#include "ffs_avx_impl.h" // IWYU pragma: export
#else
#include "ffs_generic_impl.h" // IWYU pragma: export
#endif
