#include <gtest/gtest.h>

extern "C" {
#include "prand32.h"
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
