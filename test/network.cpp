#include <gtest/gtest.h>

extern "C" {
#include "cneuron/cneuron.h"
}

#include <math.h>

#include "test_utils.h"

TEST(NetworkTest, RandomFloat) {
    float test = randf(1.0f, 0.0f);
    bool same = true;
    for (int i = 0; i < 10; i++) {
        if (test != randf(1.0f, 0.0f)) {
            same = false;
            break;
        }
    }
    ASSERT_FALSE(same);
}

TEST(NetworkTest, GetNeuralNetwork) {
    size_t layer_length = 3;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 2;
    layer_lengths[1] = 3;
    layer_lengths[2] = 4;
    size_t inputs_length = 2;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, inputs_length, &sigmoid);

    ASSERT_NE(nn, nullptr);
    ASSERT_EQ(nn->length, layer_length);
    ASSERT_EQ(nn->inputs_length, inputs_length);
    ASSERT_EQ(nn->activation_function, &sigmoid);
    ASSERT_NE(nn->layers, nullptr);
    for (size_t i = 0; i < layer_length; i++) {
        ASSERT_EQ(nn->layers[i].length, layer_lengths[i]);
    }

    free(nn);
    free(layer_lengths);
}

TEST(NetworkTest, FreeDataset) {
    size_t layer_length = 1;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 2;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, 2, nullptr);

    free(nn);
    // No crash
    free(layer_lengths);
}

TEST(NetworkTest, ComputeNetwork) {
    size_t layer_length = 1;
    size_t inputs_length = 1;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 1;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, inputs_length, &sigmoid);

    float *inputs = (float *)malloc(sizeof(float) * inputs_length);
    inputs[0] = 0.2f;

    nn->layers[0].weights[0] = 0.5f;
    nn->layers[0].bias[0] = 0.3f;

    compute_network(nn, inputs);

    ASSERT_FLOAT_EQ(nn->layers[0].output[0], 0.59868766f);

    free(inputs);
    free(nn);
    free(layer_lengths);
}

TEST(NetworkTest, Softmax) {
    size_t layer_length = 1;
    size_t inputs_length = 1;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 3;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, inputs_length, &sigmoid);

    nn->layers[0].output[0] = 0.2f;
    nn->layers[0].output[1] = 0.3f;
    nn->layers[0].output[2] = 0.5f;

    ASSERT_FLOAT_EQ(softmax(nn, 0), 0.28943311f);
    ASSERT_FLOAT_EQ(softmax(nn, 1), 0.31987305f);
    ASSERT_FLOAT_EQ(softmax(nn, 2), 0.39069383f);

    free(nn);
    free(layer_lengths);
}

// Output layer only
TEST(NetworkTest, StochasticGDSingleLayer) {
    dataset *test_dataset = get_or();

    // Create network
    size_t layer_length = 1;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 2;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, test_dataset->inputs_length, &sigmoid);

    for (size_t i = 0; i < 50000; i++) {
        for (size_t j = 0; j < test_dataset->length; j++) {
            stochastic_gd(nn, 0.03f, &test_dataset->datas[randnum_u32(test_dataset->length, 0)]);
        }
        if (i % 10000 == 0) {
            printf("Single layer learn cost: %f\n", cost(nn, test_dataset, test_dataset->length));
        }
    }

    ASSERT_LE(cost(nn, test_dataset, test_dataset->length), 0.2f);
    ASSERT_GE(test_network_percent(nn, test_dataset), 90.0f);

    free(test_dataset);
    free(nn);
    free(layer_lengths);
}

TEST(NetworkTest, StochasticGDTests) {
    dataset *test_dataset = get_xor();

    // Multi layer test
    size_t layer_length = 2;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 4;
    layer_lengths[1] = 2;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, test_dataset->inputs_length, &sigmoid);

    for (size_t i = 0; i < 500000; i++) {
        for (size_t j = 0; j < test_dataset->length; j++) {
            stochastic_gd(nn, 0.001f, &test_dataset->datas[randnum_u32(test_dataset->length, 0)]);
        }
        if (i % 100000 == 0) {
            printf("Stochastic Multi layer learn cost: %f\n", cost(nn, test_dataset, test_dataset->length));
        }
    }

    ASSERT_LE(cost(nn, test_dataset, test_dataset->length), 0.2f);
    ASSERT_GE(test_network_percent(nn, test_dataset), 90.0f);

    free(nn);
    free(layer_lengths);

    // Non-linearly separable test
    layer_length = 1;
    layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 2;
    nn = get_neural_network(layer_length, layer_lengths, test_dataset->inputs_length, &sigmoid);

    for (size_t i = 0; i < 50000; i++) {
        for (size_t j = 0; j < test_dataset->length; j++) {
            stochastic_gd(nn, 0.03f, &test_dataset->datas[randnum_u32(test_dataset->length, 0)]);
        }
        if (i % 10000 == 0) {
            printf("Stochastic Non-linearly separable learn cost: %f\n", cost(nn, test_dataset, test_dataset->length));
        }
    }

    ASSERT_GE(cost(nn, test_dataset, test_dataset->length), 0.2f);
    ASSERT_LE(test_network_percent(nn, test_dataset), 90.0f);

    free(nn);
    free(layer_lengths);
    free(test_dataset);
}

TEST(NetworkTest, MiniBatchGDTests) {
    dataset *test_dataset = get_xor();

    // Multi layer test
    size_t layer_length = 2;
    size_t *layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 4;
    layer_lengths[1] = 2;
    neural_network *nn = get_neural_network(layer_length, layer_lengths, test_dataset->inputs_length, &sigmoid);

    for (size_t i = 0; i < 2000000; i++) {
        dataset *batch_dataset = get_random_dataset_sample(test_dataset, test_dataset->length);
        mini_batch_gd(nn, 0.001f, batch_dataset);
        free(batch_dataset);
        if (i % 200000 == 0) {
            printf("Mini Batch Multi layer learn cost: %f\n", cost(nn, test_dataset, test_dataset->length));
        }
    }

    ASSERT_LE(cost(nn, test_dataset, test_dataset->length), 0.2f);
    ASSERT_GE(test_network_percent(nn, test_dataset), 90.0f);

    free(nn);
    free(layer_lengths);

    // Non-linearly separable test
    layer_length = 1;
    layer_lengths = (size_t *)malloc(sizeof(size_t) * layer_length);
    layer_lengths[0] = 2;
    nn = get_neural_network(layer_length, layer_lengths, test_dataset->inputs_length, &sigmoid);

    for (size_t i = 0; i < 100000; i++) {
        dataset *batch_dataset = get_random_dataset_sample(test_dataset, test_dataset->length);
        mini_batch_gd(nn, 0.001f, batch_dataset);
        free(batch_dataset);
        if (i % 20000 == 0) {
            printf("Mini Batch Non-linearly separable learn cost: %f\n", cost(nn, test_dataset, test_dataset->length));
        }
    }

    ASSERT_GE(cost(nn, test_dataset, test_dataset->length), 0.2f);
    ASSERT_LE(test_network_percent(nn, test_dataset), 90.0f);

    free(nn);
    free(layer_lengths);
    free(test_dataset);
}
