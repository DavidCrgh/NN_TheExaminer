#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include "include/neural_network.h"
  
void serialize(struct neural_network_t_ * input1){
    FILE *outfile; 
      
    // open file for writing 
    outfile = fopen ("nn.txt", "w"); 
    if (outfile == NULL) 
    { 
        fprintf(stderr, "\nError opend file\n"); 
        exit (1); 
    }
      
    // write struct to file 
    fwrite (&input1, sizeof(struct neural_network_t_), 1, outfile); 
      
    if(fwrite != 0)  
        printf("contents to file written successfully !\n"); 
    else 
        printf("error writing file !\n"); 
  
    // close file 
    fclose (outfile);
}

int deserialize(){
    FILE *infile; 
    struct neural_network_t_ input; 
      
    // Open person.dat for reading 
    infile = fopen ("nn.txt", "r"); 
    if (infile == NULL) 
    { 
        fprintf(stderr, "\nError opening file\n"); 
        return 0;
    } 
      
    // read file contents till end of file 
    if (fread(&input, sizeof(struct neural_network_t_), 1, infile) == 1){
        fclose (infile); 
        return 1;
    }
    else
    {
        fclose (infile); 
        return 0;
    }
}