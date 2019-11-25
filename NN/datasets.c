#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

#include "include/datasets.h"

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

/**
 * Read labels from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 */
char * get_labels(const char * path, uint32_t * number_of_labels)
{
    DIR *dir;
    struct dirent *ent;
    char labels [*number_of_labels]; 

    char full_path[PATH_MAX + 1];
    realpath(path, full_path);
    strcat(full_path, "/");
    
    if ((dir = opendir (path)) != NULL) {
    /* print all the files and directories within directory */
        memset(labels, 0, sizeof(labels));
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0] != '.'){
                char letter = ent->d_name[0];
                strncat(labels, &letter,1);
            }    
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }

    return labels;
}

/**
 * Read images from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 */
image_t * get_images(const char * path, uint32_t * number_of_images)
{
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
                strcat(buffer, full_path);
                strcat(buffer, ent->d_name);
                printf ("%s\n", buffer);
                free(buffer);
            }    
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
    
    return NULL;
}

dataset_t * get_dataset(const char * image_path, const char * label_path)
{
    dataset_t * dataset;
    uint32_t number_of_images, number_of_labels;

    dataset = calloc(1, sizeof(dataset_t));

    if (NULL == dataset) {
        return NULL;
    }

    dataset->images = get_images(image_path, &number_of_images);

    if (NULL == dataset->images) {
        free_dataset(dataset);
        return NULL;
    }

    dataset->labels = get_labels(label_path, &number_of_labels);

    if (NULL == dataset->labels) {
        free_dataset(dataset);
        return NULL;
    }

    if (number_of_images != number_of_labels) {
        fprintf(stderr, "Number of images does not match number of labels (%d != %d)\n", number_of_images, number_of_labels);
        free_dataset(dataset);
        return NULL;
    }

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
