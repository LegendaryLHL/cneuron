#include <gtest/gtest.h>

extern "C" {
    #include "data/data.h"
}

TEST(DataTest, CreateData) {
    data_t *data = (data_t *)malloc(sizeof(data_t));
    data->inputs = (float *)malloc(sizeof(float) * 10);
    for (int i = 0; i < 10; i++) {
        data->inputs[i] = static_cast<float>(i); 
    }
    free_data(data);
    // No crash
}

TEST(DataTest, GetDatasetFileNotFound) {
    unsigned int dataset_length = 0;
    unsigned int inputs_length = 0;
    data_t **dataset = get_dataset("non_existent_file.dat", &dataset_length, &inputs_length);
    ASSERT_EQ(dataset, nullptr);
}

TEST(DataTest, GetDatasetValidFile) {
    unsigned int dataset_length = 0;
    unsigned int inputs_length = 0;
    data_t **dataset = get_dataset("data/mnist/mnist_train.dat", &dataset_length, &inputs_length);
    ASSERT_NE(dataset, nullptr);
    ASSERT_GT(dataset_length, 0);
    ASSERT_GT(inputs_length, 0);

    ASSERT_NE(dataset[0], nullptr);
    ASSERT_NE(dataset[0]->inputs, nullptr);

    free_dataset(dataset, dataset_length);
}

TEST(DataTest, FreeDataset) {
    unsigned int dataset_length = 0;
    unsigned int inputs_length = 0;
    data_t **dataset = get_dataset("data/mnist/mnist_test.dat", &dataset_length, &inputs_length);

    free_dataset(dataset, dataset_length);
    // No crash
}

TEST(DataTest, FreeData) {
    data_t *data = (data_t *)malloc(sizeof(data_t));
    data->inputs = (float *)malloc(sizeof(float) * 10);

    free_data(data);
    // No crash
}


TEST(DataTest, CopyData) {
    unsigned int dataset_length = 0;
    unsigned int inputs_length = 0;
    data_t **dataset = get_dataset("data/mnist/mnist_test.dat", &dataset_length, &inputs_length);

    data_t *data_copy = get_data_copy(dataset[0], inputs_length);
    ASSERT_NE(data_copy, nullptr);
    ASSERT_NE(data_copy->inputs, nullptr);

    for (int i = 0; i < inputs_length; i++) {
        ASSERT_FLOAT_EQ(data_copy->inputs[i], dataset[0]->inputs[i]);
    }

    ASSERT_FLOAT_EQ(data_copy->neuron_index, dataset[0]->neuron_index);

    free_data(data_copy);
    free_dataset(dataset, inputs_length);
}

TEST(DataTest, RotateData) {
    data_t *data = (data_t *)malloc(sizeof(data_t));
    data->inputs = (float *)malloc(sizeof(float) * 9);
    for (int i = 0; i < 9; ++i) {
        data->inputs[i] = static_cast<float>(i);
    }

    rotate_data(data, 3, 3, 90);
    ASSERT_FLOAT_EQ(data->inputs[0], 7.0f);
    ASSERT_FLOAT_EQ(data->inputs[2], 1.0f);
    ASSERT_FLOAT_EQ(data->inputs[4], 5.0f);

    free(data->inputs);
    free(data);
}

// More functions

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
