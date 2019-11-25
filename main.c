#include "NN/datasets.c"

int main(){
    char *path = "img/";
    uint32_t number = count_files(path);

    get_images(path, &number);
}