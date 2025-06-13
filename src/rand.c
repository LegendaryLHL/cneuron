#include "rand.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "shishua/shishua-sse2.h"

static prng_state __randstate;

__attribute__((constructor)) static void auto_seed_rand(void) {
    static uint64_t seed[4] = {0x9E3779B97F4A7C15, 0xF39CC0605CEDC834, 0x1082276BF3A27251, 0xF86C6A11D0C18E95};
    prng_init(&__randstate, seed);
    prng_gen(&__randstate, randc.buf, 1024);
}

uint8_t randnum_u8(struct rand_chunk *randc, uint8_t range, uint8_t offset) {
    assert(max > min);
    assert(randc->count <= 1023);
    uint8_t randnum = ((uint16_t)randc->buf[randc->count] * range) >> 8;
    randnum += offset;
    randc->count++;
    if (randc->count > 1023) {
	randc->count = 0;
	prng_gen(&__randstate, randc->buf, 1024);
    }
    return randnum;
}

uint32_t randnum_u32(struct rand_chunk *randc, uint32_t range, uint32_t offset) {
    assert(max > min);
    assert(randc->count <= 1020);
    uint32_t value;
    // uint32_t value = randc->buf[randc->count++] << 24 | randc->buf[randc->count++] << 16 | randc->buf[randc->count++] << 8 | randc->buf[randc->count++];
    memcpy(&value, &randc->buf[randc->count], sizeof(value));
    uint32_t randnum = ((uint64_t)value * range) >> 32;
    randnum += offset;
    // randc->count += sizeof(uint32_t);
    if (randc->count > 1020) {
	randc->count = 0;
	prng_gen(&__randstate, randc->buf, 1024);
    }
    return randnum;
}

float randf(struct rand_chunk *randc, float range, float offset) {
    assert(range);
    assert(randc->count <= 1021);
    // uint32_t value;
    // memcpy(&value, &randc->buf[randc->count], 4);
    uint32_t value = randc->buf[randc->count++] << 16 | randc->buf[randc->count++] << 8 | randc->buf[randc->count++];
    // value |= 0x3F800000;
    // float randfloat;
    // randfloat = *(float *)&value;
    // memcpy(&randfloat, &value, sizeof(randfloat));
    // randfloat = (randfloat - 1) * range + offset;
    float randfloat = (value) / 16777216.0f * range + offset;
    if (randc->count > 1021) {
	randc->count = 0;
	prng_gen(&__randstate, randc->buf, 1024);
    }
    return randfloat;
}



// NOTE: example

// int main() {
//     prng_state state;
//     static uint64_t seed[4] = {0x9E3779B97F4A7C15, 0xF39CC0605CEDC834, 0x1082276BF3A27251, 0xF86C6A11D0C18E95};
//     struct rand_chunk randc = {0};
//     prng_init(&state, seed);
//     prng_gen(&state, randc.buf, 256);
//
//     for (int i = 0; i < 1000; i++) {
// 	float randfloat = randf(&randc, 2, -1);
// 	if(randc.count > 256 - sizeof(randfloat)) { //NOTE: always check randc.count b4 using function
// 	    randc.count = 0;
// 	    prng_gen(&state, randc.buf, 256);
// 	}
// 	printf("%f, ", randfloat);
//     }
//     return 0;
// }
