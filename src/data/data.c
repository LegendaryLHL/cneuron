#include "data/data.h"

#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKGROUND_VALUE 0.0f

dataset_t *get_dataset(const char *filename) {
    assert(filename);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error opening %s for reading data set\n", filename);
        return NULL;
    }

    dataset_t *dataset = malloc(sizeof(dataset_t));
    if (!dataset) {
        fclose(file);
        return NULL;
    }

    if (fread(&dataset->length, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "Error reading dataset length from %s\n", filename);
        free(dataset);
        fclose(file);
        return NULL;
    }

    dataset->datas = calloc(dataset->length, sizeof(data_t *));
    if (!dataset->datas) {
        free(dataset);
        fclose(file);
        return NULL;
    }

    if (fread(&dataset->inputs_length, sizeof(uint64_t), 1, file) != 1) {
        fprintf(stderr, "Error reading inputs_length from %s\n", filename);
        free(dataset);
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < dataset->length; i++) {
        data_t *data = malloc(sizeof(data_t));
        if (!data) {
            goto cleanup;
        }

        data->inputs = malloc(sizeof(float) * dataset->inputs_length);
        if (!data->inputs) {
            free(data);
            goto cleanup;
        }

        size_t read_inputs = fread(data->inputs, sizeof(float), dataset->inputs_length, file);
        if (read_inputs != dataset->inputs_length) {
            fprintf(stderr, "Error reading inputs_length from %s. Expected: %zu. But found: %zu\n", filename, dataset->inputs_length, read_inputs);
            free_data(data);
            goto cleanup;
        }

        if (fread(&(data->expected_index), sizeof(uint64_t), 1, file) != 1) {
            fprintf(stderr, "Error reading expected_index from %s\n", filename);
            free_data(data);
            goto cleanup;
        }

        dataset->datas[i] = data;
    }

    fclose(file);

    return dataset;

cleanup:
    free_dataset(dataset);
    fclose(file);
    return NULL;
}

void free_dataset(dataset_t *dataset) {
    if (!dataset) {
        return;
    }
    for (size_t i = 0; i < dataset->length; i++) {
        free_data(dataset->datas[i]);
    }
    free(dataset->datas);
    free(dataset);
}

void free_data(data_t *data) {
    if (!data) {
        return;
    }
    free(data->inputs);
    free(data);
}

data_t *get_data_copy(const data_t *data, size_t inputs_length) {
    assert(data);
    assert(data->inputs);
    assert(inputs_length > 0);

    data_t *copy = malloc(sizeof(data_t));
    if (!copy) {
        return NULL;
    }

    copy->expected_index = data->expected_index;

    size_t inputs_size = sizeof(float) * inputs_length;
    copy->inputs = malloc(inputs_size);
    if (!copy->inputs) {
        free(copy);
        return NULL;
    }

    memcpy(copy->inputs, data->inputs, inputs_size);

    return copy;
}

void rotate_data(data_t *data, int width, int height, float angle) {
    assert(data);
    assert(data->inputs);
    assert(width > 0 && height > 0);

    float rad = angle * M_PI / 180.0f;
    float cos_angle = cos(rad);
    float sin_angle = sin(rad);
    float *new_inputs = malloc(sizeof(float) * width * height);
    if (!new_inputs) {
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int center_x = round(width / 2.0f);
            int center_y = round(height / 2.0f);
            int src_x = round((x - center_x) * cos_angle + (y - center_y) * sin_angle + center_x);
            int src_y = round((y - center_y) * cos_angle - (x - center_x) * sin_angle + center_y);

            if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
                new_inputs[y * width + x] = data->inputs[src_y * width + src_x];
            } else {
                new_inputs[y * width + x] = BACKGROUND_VALUE;
            }
        }
    }

    free(data->inputs);
    data->inputs = new_inputs;
}

void scale_data(data_t *data, int width, int height, float scale) {
    assert(data);
    assert(data->inputs);
    assert(width > 0 && height > 0);

    int scale_width = round(width * scale);
    int scale_height = round(height * scale);
    float *new_inputs = malloc(sizeof(float) * width * height);
    if (!new_inputs) {
        return;
    }

    int offset_x = round((scale_width - width) / 2.0f);
    int offset_y = round((scale_height - height) / 2.0f);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int scaled_x = x + offset_x;
            int scaled_y = y + offset_y;

            int src_x = round(scaled_x / scale);
            int src_y = round(scaled_y / scale);

            if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
                new_inputs[y * width + x] = data->inputs[src_y * width + src_x];
            } else {
                new_inputs[y * width + x] = BACKGROUND_VALUE;
            }
        }
    }

    free(data->inputs);
    data->inputs = new_inputs;
}

void offset_data(data_t *data, int width, int height, float offset_x, float offset_y) {
    assert(data);
    assert(data->inputs);
    assert(width > 0 && height > 0);

    float *new_inputs = malloc(sizeof(float) * width * height);
    if (!new_inputs) {
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float new_x = x - offset_x;
            float new_y = y - offset_y;

            int src_x = round(new_x);
            int src_y = round(new_y);
            if (src_x >= 0 && src_x < width && src_y >= 0 && src_y < height) {
                new_inputs[y * width + x] = data->inputs[src_y * width + src_x];
            } else {
                new_inputs[y * width + x] = BACKGROUND_VALUE;
            }
        }
    }
    free(data->inputs);
    data->inputs = new_inputs;
}

void noise_data(data_t *data, size_t inputs_length, float noise_factor, float probability) {
    assert(data);
    assert(data->inputs);
    assert(inputs_length > 0);

    for (size_t i = 0; i < inputs_length; i++) {
        float random_value = rand() / (float)RAND_MAX;
        if (random_value <= probability) {
            float noise = (rand() / (float)RAND_MAX * noise_factor);
            float new_value = data->inputs[i] + noise;

            data->inputs[i] = fmin(new_value, 1.0f);
        }
    }
}

float output_expected(size_t expected_index, const data_t *data) {
    assert(data);

    if (data->expected_index == expected_index) {
        return 1.0f;
    } else {
        return 0.0f;
    }
}
