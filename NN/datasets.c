#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

#include "include/datasets.h"
#include "read_file.c"

/**
 * Convert from the big endian format in the dataset if we're on a little endian
 * machine.
 */
/*uint32_t map_uint32(uint32_t in)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return (
        ((in & 0xFF000000) >> 24) |
        ((in & 0x00FF0000) >>  8) |
        ((in & 0x0000FF00) <<  8) |
        ((in & 0x000000FF) << 24)
    );
#else
    return in;
#endif
}*/

#define ARRAY_SIZE(x) ((int)(sizeof(x) / sizeof((x)[0])))

uint8_t get_label_code(char letter){
    if(letter == 'X'){ // Label de error
        return 6;
    } else if(letter == 'S') { // Label de espacio en blanco
        return 7;
    } else
    {
        return letter - 65; //ASCII de la A para empezar en 0
    }
    
}

uint32_t count_files(const char *path){
    uint32_t file_count = 0;
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir(path); /* There should be error handling after this */
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            file_count++;
        }
    }
    closedir(dirp);
    return file_count;
}

/**
 * Read labels from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 */
uint8_t * get_labels(const char * path, uint32_t number_of_labels)
{
    DIR *dir;
    struct dirent *ent;
    uint8_t *labels = malloc(number_of_labels * sizeof(uint8_t));
    int i = 0;
    
    if ((dir = opendir (path)) != NULL) {
    /* print all the files and directories within directory */
        memset(labels, 0, sizeof(labels));
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0] != '.'){
                uint8_t code = get_label_code(ent->d_name[0]);
                labels[i] = code;
                i++;
                //char letter = ent->d_name[0];
                //strncat(labels, &letter,1);
            }    
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return NULL;
    }

    return labels;
}

/**
 * Read images from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 */
image_t * get_images(const char * path, uint32_t number_of_images)
{
    image_t *images = malloc(number_of_images * sizeof(image_t));
    int i = 0; // Contador de imagenes
    //image_t images[number_of_images];

    DIR *dir;
    struct dirent *ent;

    char full_path[PATH_MAX + 1];
    realpath(path, full_path);
    strcat(full_path, "/");
    
    if ((dir = opendir (path)) != NULL) {
    /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0] != '.'){
                char *buffer = (char *)calloc(PATH_MAX + 1, sizeof(char));

                // Construye commando para invocar al script de python con el path de la imagen
                strcat(buffer, "python3 Preprocessor/preproc.py '");
                strcat(buffer, full_path);
                strcat(buffer, ent->d_name);
                strcat(buffer, "'");
                printf ("%s\n", buffer);

                // Ejecuta el comando que corre el script
                system(buffer);

                uint8_t px[IMAGE_SIZE];
                read_file(px);
                full_path[0] = '/'; //Un bug raro ocasiona que el primer caracter sea nulo despues de llamara a read_file

                if(i < number_of_images){
                    memcpy(images[i].pixels, px, IMAGE_SIZE);
                }
                
                // Libera la memoria
                free(buffer);
                i++;
            }    
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return NULL;
    }

    return images;
}

dataset_t * get_dataset(const char * image_path, const char * label_path)
{
    dataset_t * dataset;
    uint32_t number_of_images = count_files(image_path);

    dataset = calloc(1, sizeof(dataset_t));

    if (NULL == dataset) {
        return NULL;
    }

    dataset->images = get_images(image_path, number_of_images);

    if (NULL == dataset->images) {
        free_dataset(dataset);
        return NULL;
    }

    dataset->labels = get_labels(label_path, number_of_images);

    if (NULL == dataset->labels) {
        free_dataset(dataset);
        return NULL;
    }

    int MAX = 16;

    printf("dataset->labels\n");
    for(int i = 0; i < MAX; i++){
        printf("%d : %d\n", i, dataset->labels[i]);
    }

    /*if (number_of_images != number_of_labels) {
        fprintf(stderr, "Number of images does not match number of labels (%d != %d)\n", number_of_images, number_of_labels);
        free_dataset(dataset);
        return NULL;
    }*/

    dataset->size = number_of_images;

    return dataset;
}

/**
 * Free all the memory allocated in a dataset. This should not be used on a
 * batched dataset as the memory is allocated to the parent.
 */
void free_dataset(dataset_t * dataset)
{
    free(dataset->images);
    free(dataset->labels);
    free(dataset);
}

/**
 * Fills the batch dataset with a subset of the parent dataset.
 */
int batch(dataset_t * dataset, dataset_t * batch, int size, int number)
{
    int start_offset;

    start_offset = size * number;

    if (start_offset >= dataset->size) {
        return 0;
    }

    batch->images = &dataset->images[start_offset];
    batch->labels = &dataset->labels[start_offset];
    batch->size = size;

    if (start_offset + batch->size > dataset->size) {
        batch->size = dataset->size - start_offset;
    }

    return 1;
}
