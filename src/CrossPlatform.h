#ifndef PORTABLE_CTZ_H
#define PORTABLE_CTZ_H

#include <cstdint>

#ifdef _MSC_VER
#  include <intrin.h>
#  pragma intrinsic(_BitScanForward)
#  pragma intrinsic(_BitScanForward64)
#endif

namespace portable {
    inline int ctzll(std::uint64_t x) noexcept
    {
#if defined(__clang__) || defined(__GNUC__)
        return __builtin_ctzll(x);

#elif defined(_MSC_VER) && defined(_M_X64)
        unsigned long idx;
        _BitScanForward64(&idx, x);
        return static_cast<int>(idx);
#else
#   error "portable::ctzll is not implemented for this compiler."
#endif
    }

    inline int popcount(std::uint64_t x) noexcept
    {
#if defined(__clang__) || defined(__GNUC__)
        return std::popcount(x);
#elif defined(_MSC_VER) && defined(_M_X64)
        return __popcnt64(x);
#else
#   error "portable::popcount is not supported on this compiler"
#endif
    }
}

#endif
