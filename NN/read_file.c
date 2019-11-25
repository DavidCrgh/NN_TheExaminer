#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAMANIO_BUFFER 10000

int read_file(int *pixels) {
    char buffer[TAMANIO_BUFFER];
    //int pixels[576];
    FILE *file;

    file = fopen("blocks_array.txt", "r");

    if (!file) {
        printf("¡No se pudo abrir el archivo %s!", "blocks_array.txt");
        return EXIT_FAILURE;
    }

    while (!feof(file)) {
        fread(buffer, sizeof(char), sizeof(buffer), file);
    }

    int iterator = 0;

    memset(pixels, 0, sizeof(pixels));

    char * token = strtok(buffer, "\n");
    while( token != NULL ) {
        pixels[iterator] = atoi(token);
        token = strtok(NULL, "\n");
        iterator++;
    }

    iterator = 0;

    /*while (iterator < 576){
        printf("%d\n",pixels[iterator]);
        iterator++;
    }*/
    
    fclose(file);
    return EXIT_SUCCESS;
}