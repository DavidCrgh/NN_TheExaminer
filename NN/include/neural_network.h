#ifndef NEURAL_NETWORK_H_
#define NEURAL_NETWORK_H_

#include "datasets.h"

typedef struct neural_network_t_ {
    float b[LABELS];
    float W[LABELS][IMAGE_SIZE];
} neural_network_t;

typedef struct neural_network_gradient_t_ {
    float b_grad[LABELS];
    float W_grad[LABELS][IMAGE_SIZE];
} neural_network_gradient_t;

void neural_network_random_weights(neural_network_t * network);
void neural_network_hypothesis(image_t * image, neural_network_t * network, float activations[LABELS]);
float neural_network_gradient_update(image_t * image, neural_network_t * network, neural_network_gradient_t * gradient, uint8_t label);
float neural_network_training_step(dataset_t * dataset, neural_network_t * network, float learning_rate);

#endif
