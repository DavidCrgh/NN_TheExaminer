/**
 * Codigo adaptado del repositorio mnist-neural-network-plain-c
 * creado originalmente por AndrewCarterUK
 * 
 * Link de GitHub: https://github.com/AndrewCarterUK/mnist-neural-network-plain-c
 * 
 * Archivo original: mnist.c
*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "io_utils.c"

#include "NN/datasets.c"
#include "NN/neural_network.c"
#include "NN/serializer.c"

#define STEPS 3000
#define BATCH_SIZE 100

const char * train_images_file = "training/";
const char * test_images_file = "test/";


/**
 * calculate_accuracy
 * Calcula la exactitud de las predicciones realizadas por la red para el dataset de pruebas.
 *
 * Desc. original:
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
    printf("\033[0;31m"); // Activa rojo
    train_dataset = get_dataset(train_images_file, train_images_file);
    test_dataset = get_dataset(test_images_file, test_images_file);
    printf("\033[0m"); // Desactiva rojo 

    // Calculate how many batches (so we know when to wrap around)
    batches = train_dataset->size / BATCH_SIZE;

    printf("\033[0;32m"); // Activa verde
    for (i = 0; i < STEPS; i++) {
        // Initialise a new batch
        make_batch(train_dataset, &batch, 100, i % batches);

        // Run one step of gradient descent and calculate the loss
        loss = neural_network_training_step(&batch, network, 0.5);

        // Calculate the accuracy using the whole test dataset
        accuracy = calculate_accuracy(test_dataset, network);

        printf("Step %04d\tAverage Loss: %.2f\tAccuracy: %.3f\n", i, loss / batch.size, accuracy);
    }
    printf("\033[0m"); // Desactiva verde

    printf("\nEntrenamiento finalizado.\n");

    // Cleanup
    free_dataset(train_dataset);
    free_dataset(test_dataset);

    serialize(network);
    printf("Red serializada y guardada!\n\n");
}

/**
 * main
 * Ejecuta el programa principal de linea de comandos.
 */
int main(int argc, char *argv[])
{
    neural_network_t network;
    int seleccion = 0;

    // Hacer un print cuando no se pudo serializar la red para evitar equivocarse
    // Initialise weights and biases with random values
    if (deserialize(&network) == 0){
        neural_network_random_weights(&network);
        printf("No se encontro una red para de-serializar, se iniciara una red con pesos aleatorios!\n\n");
    }

    print_header();
    while(seleccion != 3){
        seleccion = run_menu();

        switch (seleccion)
        {
        case 1:
            printf("Entrenando red...\n\n");
            train_network(&network);
            break;
        case 2: 
            printf("Clasificador de imagenes\n");
            char str[PATH_MAX];
            getLine("Ingrese un path de imagen: \n>> ", str, sizeof(str));

            printf("\033[0;31m"); // Activa rojo
            int label = classify_image(str, &network);
            printf("\033[0m"); // Desactiva rojo 

            print_label(label);
            break;
        case -1:
            printf("Error: Entrada invalida, intente de nuevo.\n");
            break;         
        default:
            break;
        }

        printf("=========================================================\n\n");
    }

    return 0;
}
