#ifndef PRAND32_H
#define PRAND32_H

#include <stdint.h>

static uint32_t state = 123456;

static inline uint32_t prand32(void) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

static inline void sprand32(uint32_t seed) {
    state = seed;
}

// 0.0f - 1.0f
static inline float prand32f(void) {
    return (prand32() >> 8) * (1.0f / 16777216.0f);
}

#endif
