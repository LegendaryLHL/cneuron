#ifndef PRAND32_H
#define PRAND32_H

#include <assert.h>
#include <stdint.h>

extern uint32_t state;

uint32_t prand32(void);

// 0.0f - 1.0f
float prand32f(void);

float prand32f_range(float min, float max);

#endif
