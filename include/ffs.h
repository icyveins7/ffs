#pragma once

#ifdef __AVX__
#include "ffs_avx_impl.h"
#else
#include "ffs_generic_impl.h"
#endif