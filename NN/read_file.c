#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAMANIO_BUFFER 10000

int main() {

    char buffer[TAMANIO_BUFFER];
    float pixels[576];
    FILE *file;

    size_t readBytes;

    file = fopen("prueba.txt", "r");

    if (!file) {
        printf("¡No se pudo abrir el archivo %s!", "prueba.txt");
        return EXIT_FAILURE;
    }

    while (!feof(file)) {
        readBytes = fread(buffer, sizeof(char), sizeof(buffer), file);
    }

    int iterator = 0;

    char * token = strtok(buffer, "\n");
    while( token != NULL ) {
        pixels[iterator] = atof(token);
        token = strtok(NULL, "\n");
        iterator++;
    }

    while (readBytes > 0){
        printf("%f\n",pixels[readBytes]);
        readBytes--;
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}