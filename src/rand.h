#ifndef SHISHUA_RAND_H
#define SHISHUA_RAND_H

#include <stddef.h>
#include <stdint.h>
#include "../external/shishua/shishua.h"

struct rand_chunk {
    size_t count;
    uint8_t buf[1024];  // NOTE: must be multiple of 256
};

uint8_t randnum_u8(struct rand_chunk *randc, uint8_t range, uint8_t offset);
uint32_t randnum_u32(struct rand_chunk *randc, uint32_t range, uint32_t offset);
float randf(struct rand_chunk *randc, float range, float offset);
static struct rand_chunk randc = {0};

#endif
