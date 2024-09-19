#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    
    int exit_state = 0;
    int result;
    char *archivo;
    
    if(argc==1){
        printf("remover_archivo: Falta un operando. \n");
        exit_state = 1;
    }else{ 
        archivo = *(argv+1);
        result = remove(archivo);
        if(result==-1){
            printf("remover_archivo: El archivo no existe, no se puede eliminar '%s' \n", archivo);
            exit_state = 1;
        }
    }
    
    return exit_state;
}

