
#ifndef CHESS_RANDOM_H
#define CHESS_RANDOM_H

#include <random>
#include <stdint.h>

inline void seedRandom() {
    std::srand('j' + 'h' + 'm' + 'r' + 33);
}

inline uint64_t random_uint64() {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(std::rand()) & 0xFFFF; u2 = (uint64_t)(std::rand()) & 0xFFFF;
    u3 = (uint64_t)(std::rand()) & 0xFFFF; u4 = (uint64_t)(std::rand()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

#endif