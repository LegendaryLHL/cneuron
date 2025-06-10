#include <gtest/gtest.h>

extern "C" {
#include "cneuron/cneuron.h"
}

#include <math.h>

#include "test_utils.h"

TEST(LinearAlgebraTest, VectorApplyActivation) {
    size_t length = 3;
    float *a = (float *)malloc(sizeof(float) * length);
    float *b = (float *)malloc(sizeof(float) * length);

    a[0] = 1.0;
    a[1] = -1.2;
    a[2] = -0.2;

    vector_apply_activation(a, b, length, sigmoid, false);

    for (size_t i = 0; i < length; i++) {
        ASSERT_FLOAT_EQ(b[i], sigmoid(a[i], false));
    }

    free(a);
    free(b);
}
