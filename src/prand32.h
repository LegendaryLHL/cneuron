#ifndef PRAND32_H
#define PRAND32_H

#include <assert.h>
#include <stdint.h>

void ensure_seeded(void);

uint32_t prand32(void);

// Select random index [0, length)
uint32_t prand32_index(uint32_t length);

// 0.0f - 1.0f
float prand32f(void);

float prand32f_range(float min, float max);

#endif
