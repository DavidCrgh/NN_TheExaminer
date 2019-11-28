/**
 * Codigo adaptado del repositorio mnist-neural-network-plain-c
 * creado originalmente por AndrewCarterUK
 * 
 * Link de GitHub: https://github.com/AndrewCarterUK/mnist-neural-network-plain-c
 * 
 * Archivo original: mnist_file.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

#include "include/datasets.h"
#include "read_file.c"

/**
 * get_image
 * Inicializa un struct de tipo image_t a partir del path especificado.
 * 
*/
image_t * get_image(const char *path){
    image_t * imagen = calloc(1, sizeof(image_t));
    char *buffer = (char *)calloc(PATH_MAX + 1, sizeof(char));

    // Construye commando para invocar al script de python con el path de la imagen
    strcat(buffer, "python3 Preprocessor/preproc.py '");
    strcat(buffer, path);
    strcat(buffer, "'");
    
    printf("\033[0;31m"); // Activa rojo
    printf ("Calling script: %s\n\n", buffer);
    printf("\033[0m"); // Desactiva rojo

    // Ejecuta el comando que corre el script
    system(buffer);

    // Lee los pixeles del archivo creado por el script y los guarda en la struct imagen
    uint8_t px[IMAGE_SIZE];
    read_file(px);
    memcpy(imagen->pixels, px, IMAGE_SIZE);

    return imagen;
}

/**
 * get_label_code
 * Convierte el char de un label a su codigo entre 0 y 7 correspondiente.}
 *
*/
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

/**
 * count_files
 * Cuenta los archivos en un directorio.
*/
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
 * get_labels
 * Obtiene un arreglo de labels del dataset encontrado en el path ingresado.
 *
 * Funcion original: get_labels
 *
 * Desc. original:
 * Read labels from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 *
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
 * get_images
 * Obtiene un arreglo de structs image_t con las imagenes encontradas en el path ingresado.
 *
 * Funcion original: get_images
 *
 * Desc. original:
 * Read images from file.
 * 
 * File format: http://yann.lecun.com/exdb/mnist/
 *
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

                // Imprimir invocaciones a strings
                printf ("Calling script %d of %d: %s\n\n", i + 1, number_of_images, buffer);

                // Ejecuta el comando que corre el script
                system(buffer);

                // Lee los pixeles del archivo creado por el script y los guarda en la struct imagen
                uint8_t px[IMAGE_SIZE];
                read_file(px);
                full_path[0] = '/'; //Un bug raro hace que el primer caracter sea nulo despues de llamara a read_file

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

/**
 * get_dataset
 * Inicializa un struct dataset_t con las imagenes y los labels encontrados en el path
 * dado.
 *
 * Func. original: mnist_get_dataset
 * Desc. original:
 * Initialize a dataset struct from the specified image and label paths.
 * 
*/
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

    dataset->size = number_of_images;

    return dataset;
}

/**
 * free_dataset
 * 
 * Desc. original:
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
 * make_batch
 *
 * Desc. original:
 * Fills the batch dataset with a subset of the parent dataset.
 */
int make_batch(dataset_t * dataset, dataset_t * batch, int size, int number)
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
