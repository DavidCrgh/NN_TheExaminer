#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>

#include "NN/datasets.c"
#include "NN/neural_network.c"
#include "NN/serializer.c"

#define STEPS 2000
#define BATCH_SIZE 100

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

const char * train_images_file = "training/";
const char * test_images_file = "test/";

static int getLine (char *prmpt, char *buff, size_t sz) {
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prmpt != NULL) {
        printf ("%s", prmpt);
        fflush (stdout);
    }
    if (fgets (buff, sz, stdin) == NULL)
        return NO_INPUT;

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return OK;
}

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

/**
 * Pasa la imagen encontrada en path a la red neuronal para clasificarla
*/
int classify_image(char *path, neural_network_t * network){
    float activations[LABELS], max_activation;
    int predict; //Indice del label con el mayor valor de activacion

    image_t *imagen = get_image(path);
    
    //Calcula las activaciones para la imagen
    neural_network_hypothesis(imagen, network, activations);

    predict = 0;
    max_activation = activations[0];

    // Obtiene el indice de la activacion mayor y lo guarda en predict
    for(int i = 0; i < LABELS; i++){
        if (max_activation < activations[i]) {
            max_activation = activations[i];
            predict = i;
        }
    }

    return predict;
}

void train_network(neural_network_t * network){
    dataset_t * train_dataset, * test_dataset;
    dataset_t batch;
    float loss, accuracy;
    int i, batches;

    // Read the datasets from the files
    train_dataset = get_dataset(train_images_file, train_images_file);
    test_dataset = get_dataset(test_images_file, test_images_file);

    // Initialise weights and biases with random values
    //neural_network_random_weights(network);

    // Calculate how many batches (so we know when to wrap around)
    batches = train_dataset->size / BATCH_SIZE;

    for (i = 0; i < STEPS; i++) {
        // Initialise a new batch
        make_batch(train_dataset, &batch, 100, i % batches);

        // Run one step of gradient descent and calculate the loss
        loss = neural_network_training_step(&batch, network, 0.5);

        // Calculate the accuracy using the whole test dataset
        accuracy = calculate_accuracy(test_dataset, network);

        printf("Step %04d\tAverage Loss: %.2f\tAccuracy: %.3f\n", i, loss / batch.size, accuracy);
    }

    // Cleanup
    free_dataset(train_dataset);
    free_dataset(test_dataset);

    //TODO: serializar la red
    serialize(network);
}

int main(int argc, char *argv[])
{
    neural_network_t network;

    // TODO: Aqui intentar de-serializar la red, si falla correr la funcion de abajo
    if (deserialize(&network)){

    } else
    {
        neural_network_random_weights(&network);
        train_network(&network);
    }

    while (1 == 1)
    {
        char str[PATH_MAX];
        getLine("Ingrese un path de imagen: \n", str, sizeof(str));
        int label = classify_image(str, &network);
        printf("Label: %d\n", label);
    }   

    return 0;
}
