#include "prand32.h"

#include "time.h"

uint32_t state = 123456;

__attribute__((constructor)) static void auto_seed_prand32(void) {
    state = (uint32_t)time(NULL);
}

inline uint32_t prand32(void) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

// 0.0f - 1.0f
inline float prand32f(void) {
    return (prand32() >> 8) * (1.0f / 16777216.0f);
}

inline float prand32f_range(float min, float max) {
    assert(min < max);

    return prand32f() * (max - min) + min;
}
