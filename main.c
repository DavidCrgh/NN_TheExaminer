#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "NN/datasets.c"
#include "NN/neural_network.c"

#define STEPS 1000
#define BATCH_SIZE 100

/**
 * Downloaded from: http://yann.lecun.com/exdb/mnist/
 */
const char * train_images_file = "training/";
//const char * train_labels_file = "data/train-labels-idx1-ubyte";
const char * test_images_file = "test/";
//const char * test_labels_file = "data/t10k-labels-idx1-ubyte";

/**
 * Calculate the accuracy of the predictions of a neural network on a dataset.
 */
float calculate_accuracy(dataset_t * dataset, neural_network_t * network)
{
    float activations[LABELS], max_activation;
    int i, j, correct, predict;

    // Loop through the dataset
    for (i = 0, correct = 0; i < dataset->size; i++) {
        // Calculate the activations for each image using the neural network
        neural_network_hypothesis(&dataset->images[i], network, activations);

        // Set predict to the index of the greatest activation
        for (j = 0, predict = 0, max_activation = activations[0]; j < LABELS; j++) {
            if (max_activation < activations[j]) {
                max_activation = activations[j];
                predict = j;
            }
        }

        // Increment the correct count if we predicted the right label
        if (predict == dataset->labels[i]) {
            correct++;
        }
    }

    // Return the percentage we predicted correctly as the accuracy
    return ((float) correct) / ((float) dataset->size);
}

int main(int argc, char *argv[])
{
    dataset_t * train_dataset, * test_dataset;
    dataset_t batch;
    neural_network_t network;
    float loss, accuracy;
    int i, batches;

    // Read the datasets from the files
    train_dataset = get_dataset(train_images_file, train_images_file);
    test_dataset = get_dataset(test_images_file, test_images_file);

    // Initialise weights and biases with random values
    neural_network_random_weights(&network);

    // Calculate how many batches (so we know when to wrap around)
    batches = train_dataset->size / BATCH_SIZE;

    for (i = 0; i < STEPS; i++) {
        // Initialise a new batch
        make_batch(train_dataset, &batch, 100, i % batches);

        // Run one step of gradient descent and calculate the loss
        loss = neural_network_training_step(&batch, &network, 0.5);

        // Calculate the accuracy using the whole test dataset
        accuracy = calculate_accuracy(test_dataset, &network);

        printf("Step %04d\tAverage Loss: %.2f\tAccuracy: %.3f\n", i, loss / batch.size, accuracy);
    }

    // Cleanup
    free_dataset(train_dataset);
    free_dataset(test_dataset);

    return 0;
}
