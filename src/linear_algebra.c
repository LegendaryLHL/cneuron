#include <assert.h>
#include <stdbool.h>
#include <cblas.h>

#include "cneuron/cneuron.h"

void matrix_multiply(const float *a, const float *b, float *c, size_t rows_a, size_t cols_a, size_t cols_b) {
    assert(a && b && c);
    cblas_sgemm(CblasColMajor, CblasNoTrans, CblasNoTrans, rows_a, cols_b, cols_a, 1.0f, a, rows_a, b, cols_a, 0.0f, c, rows_a);
}

void vector_apply_activation(const float *a, float *b, size_t length, float (*activation_function)(float, bool)) {
    assert(a && b && activation_function);

    for (size_t i = 0; i < length; i++) {
        b[i] = activation_function(a[i], false);
    }
}

void vector_add(const float *a, const float *b, float *c, size_t length) {
    assert(a && b && c);

    for (size_t i = 0; i < length; i++) {
        c[i] = a[i] + b[i];
    }
}
