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

    inline void* aligned_alloc(size_t alignment, size_t size) {
#if defined(__clang__) || defined(__GNUC__)
        return std::aligned_alloc(alignment, size);
#elif defined(_MSC_VER) && defined(_M_X64)
        return _aligned_malloc(size, alignment);
#else
#   error "portable::popcount is not supported on this compiler"
#endif
    }

    inline void aligned_free(void* memblock) {
#if defined(__clang__) || defined(__GNUC__)
        return std::free(memblock);
#elif defined(_MSC_VER) && defined(_M_X64)
        return _aligned_free(memblock);
#else
#   error "portable::popcount is not supported on this compiler"
#endif
    }
}

#endif
