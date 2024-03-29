#include <stdio.h>

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

/**
 * Imprime prmpt, lee string de tamano sz desde stdin y lo guarda en buff.
 * 
 * Retorna 0 para exito, 1 cuando no hay entrada y 2 cuando la entrada es muy
 * larga.
 * 
 * Codigo original obtenido del usuario de StackOverflow, paxdiablo
 * Link: https://stackoverflow.com/questions/4023895/how-do-i-read-a-string-entered-by-the-user-in-c
*/
static int getLine (char *prmpt, char *buff, size_t sz) {
    int ch, extra;

    // Get line with buffer overrun protection.
    if (prmpt != NULL) {
        printf ("%s", prmpt);
        fflush (stdout);
    }
    //if (fgets (buff, sz, stdin) == NULL)
      //  return NO_INPUT;
    getline(&buff, &sz, stdin);

    // If it was too long, there'll be no newline. In that case, we flush
    // to end of line so that excess doesn't affect the next call.
    if (buff[strlen(buff)-1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? TOO_LONG : OK;
    }

    // Otherwise remove newline and give string back to caller.
    buff[strlen(buff)-1] = '\0';
    return OK;
}

/**
 * Imprime el texto que aparece al inicio del programa.
*/
void print_header(){
    printf("=====================NN - The Examiner=====================\n");
    printf("Curso de Inteligencia Artificial, Tecnologico de Costa Rica.\n");
    printf("Creado por: Gerardo Gutierrez y David Valverde\n\n");
}

/**
 * Imprime la letra correspondiente al codigo de label (0-7) ingresado.
 * 
*/
void print_label(int label_code){
    char label_char;

    if(label_code == 6){
        label_char = 'X';
    }else if (label_code == 7)
    {
        label_char = 'S';
    }else
    {
        label_char = label_code + 65; //ASCII de la A
    }
    
    printf("Label retornada: %c\n", label_char);
}

/**
 * Imprime el menu principal de la aplicacion y retorna la respuesta dada.
*/
int run_menu(){
    int seleccion, resultado;

    printf("Ingrese una opcion:\n");
    printf("\t1. Entrenar red y guardar resultados\n");
    printf("\t2. Clasificar imagen\n");
    printf("\t3. Salir\n\n");
    printf(">> ");

    fflush(stdout);
    resultado = scanf("%d", &seleccion);
    fgetc(stdin);

    if(resultado == EOF || resultado == 0){
        return -1;
    }else
    {
        return seleccion;
    }
}