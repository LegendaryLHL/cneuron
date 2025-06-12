#ifndef SHISHUA_RAND_H
#define SHISHUA_RAND_H

#include "shishua.h"
#include <stdint.h>

struct rand_chunk{
    size_t count;
    uint8_t buf[128]; //NOTE: must be multiple of 128
};

uint8_t randnum_u8(struct rand_chunk *randc, uint8_t min, uint8_t max);
uint32_t randnum_u32(struct rand_chunk *randc, uint32_t min, uint32_t max);
float randf(struct rand_chunk *randc, float range, float offset);

#endif
