#include <gtest/gtest.h>

extern "C" {
    #include "data/data.h"
}

class DataTest : public ::testing::Test {
protected:
    void SetUp() override {
        dataset = nullptr;
        data_copy = nullptr;
        dataset_length = 0;
        inputs_length = 0;
    }

    void TearDown() override {}

    data_t **dataset;
    data_t *data_copy;
    unsigned int dataset_length;
    unsigned int inputs_length;
};

TEST_F(DataTest, GetDatasetFileNotFound) {
    dataset = get_dataset("non_existent_file.dat", &dataset_length, &inputs_length);
    ASSERT_EQ(dataset, nullptr);
}

TEST_F(DataTest, GetDatasetValidFile) {
    dataset = get_dataset("data/mnist/mnist_train.dat", &dataset_length, &inputs_length);
    ASSERT_NE(dataset, nullptr);
    ASSERT_GT(dataset_length, 0);
    ASSERT_GT(inputs_length, 0);
}

TEST_F(DataTest, CopyData) {
    data_copy = get_data_copy(dataset[0], inputs_length);
    // More assert here
}

TEST_F(DataTest, FreeData) {
    free_data(data_copy);
    // More assert such as checking nullptr but need to fix c
}

TEST_F(DataTest, CreateData) {
    data_t *data = (data_t *)malloc(sizeof(data_t));
    data->inputs = (float *)malloc(sizeof(float) * 10);
    free_data(data);
    // No crash check nullptr
}

TEST_F(DataTest, RotateData) {
    // Create and free data here
    data_t *data = (data_t *)malloc(sizeof(data_t));
    data->inputs = (float *)malloc(sizeof(float) * 9);
    for (int i = 0; i < 9; ++i) {
        data->inputs[i] = i + 1;
    }
    rotate_data(data, 3, 3, 90);
    ASSERT_FLOAT_EQ(data->inputs[2], 1.0f);
    free(data->inputs);
    free(data);
}

// More functions

TEST_F(DataTest, FreeDataset) {
    free_dataset(dataset, dataset_length);
    // No crash and checsk ptr
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
