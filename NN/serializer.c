#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#include "include/neural_network.h"
  
void serialize(){
    FILE *outfile; 
      
    // open file for writing 
    outfile = fopen ("person.txt", "w"); 
    if (outfile == NULL) 
    { 
        fprintf(stderr, "\nError opend file\n"); 
        exit (1); 
    } 
  
    struct neural_network_t_ input1 = {1, "rohan", "sharma"}; 
      
    // write struct to file 
    fwrite (&input1, sizeof(struct neural_network_t_), 1, outfile); 
      
    if(fwrite != 0)  
        printf("contents to file written successfully !\n"); 
    else 
        printf("error writing file !\n"); 
  
    // close file 
    fclose (outfile);
}

void deserialize(){
    FILE *infile; 
    struct neural_network_t_ input; 
      
    // Open person.dat for reading 
    infile = fopen ("person.txt", "r"); 
    if (infile == NULL) 
    { 
        fprintf(stderr, "\nError opening file\n"); 
        exit (1); 
    } 
      
    // read file contents till end of file 
    while(fread(&input, sizeof(struct neural_network_t_), 1, infile))
  
    // close file 
    fclose (infile); 
}