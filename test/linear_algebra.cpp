#include <gtest/gtest.h>

extern "C" {
#include "cneuron/cneuron.h"
}

#include <math.h>

TEST(LinearAlgebraTest, MatrixMultiply) {
    size_t rows_a = 3;
    size_t cols_a = 1;
    size_t rows_b = 1;
    size_t cols_b = 3;
    float *a = (float *)malloc(sizeof(float) * rows_a * cols_a);
    float *b = (float *)malloc(sizeof(float) * rows_b * cols_b);

    a[0] = 1.0f;
    a[1] = 2.0f;
    a[2] = 3.0f;
    b[0] = 4.0f;
    b[1] = 5.0f;
    b[2] = 6.0f;

    float *c = (float *)malloc(sizeof(float) * rows_a * cols_b);
    matrix_vector_multiply(a, b, c, rows_a, cols_a);
}
