#include "rand.h"

#include <assert.h>
#include <string.h>

uint8_t randnum_u8(struct rand_chunk *randc, uint8_t min, uint8_t max) {
    assert(max > min);
    assert(randc->count <= 127);
    uint8_t randnum = ((uint16_t)randc->buf[randc->count] * (max - min)) >> 8;
    randnum += min;
    randc->count++;
    return randnum;
}

uint32_t randnum_u32(struct rand_chunk *randc, uint32_t min, uint32_t max) {
    assert(max > min);
    assert(randc->count <= 124);
    uint32_t value;
    memcpy(&value, &randc->buf[randc->count], sizeof(value));
    uint32_t randnum = ((uint64_t)value * (max - min)) >> 32;
    randnum += min;
    randc->count += sizeof(uint32_t);
    return randnum;
}

float randf(struct rand_chunk *randc, float range, float offset) {
    assert(range);
    assert(randc->count <= 124);
    uint32_t value;
    memcpy(&value, &randc->buf[randc->count], sizeof(value));
    // float randfloat = ((float)value / UINT32_MAX) * range + offset;
    float randfloat = ((value >> 8) / 16777216.0f) * range + offset;
    randc->count += sizeof(float);
    return randfloat;
}

// NOTE: example

// int main() {
//     prng_state state;
//     static uint64_t seed[4] = {0x9E3779B97F4A7C15, 0xF39CC0605CEDC834, 0x1082276BF3A27251, 0xF86C6A11D0C18E95};
//     struct rand_chunk randc = {0};
//     prng_init(&state, seed);
//     prng_gen(&state, randc.buf, 128);
//
//     for (int i = 0; i < 1000; i++) {
// 	float randfloat = randf(&randc, 2, -1);
// 	if(randc.count > 128 - sizeof(randfloat)) { //NOTE: always check randc.count b4 using function
// 	    randc.count = 0;
// 	    prng_gen(&state, randc.buf, 128);
// 	}
// 	printf("%f, ", randfloat);
//     }
//     return 0;
// }
