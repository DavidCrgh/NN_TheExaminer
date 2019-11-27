/**
 * Codigo adaptado del repositorio mnist-neural-network-plain-c
 * creado originalmente por AndrewCarterUK
 * 
 * Link de GitHub: https://github.com/AndrewCarterUK/mnist-neural-network-plain-c
 * 
 * Archivo original: mnist_file.h
*/
#ifndef DATASETS_H_
#define DATASETS_H_

#include <stdint.h>

#define IMAGE_WIDTH 26
#define IMAGE_HEIGHT 26
#define IMAGE_SIZE IMAGE_WIDTH * IMAGE_HEIGHT
#define LABELS 8

typedef struct image_t_ {
    uint8_t pixels[IMAGE_SIZE];
} __attribute__((packed)) image_t;

typedef struct dataset_t_ {
    image_t * images;
    uint8_t * labels;
    uint32_t size;
} dataset_t;

dataset_t * get_dataset(const char * image_path, const char * label_path);
image_t * get_images(const char * path, uint32_t number_of_images);
uint8_t * get_labels(const char * path, uint32_t number_of_labels);
void free_dataset(dataset_t * dataset);
int make_batch(dataset_t * dataset, dataset_t * batch, int batch_size, int batch_number);

#endif
