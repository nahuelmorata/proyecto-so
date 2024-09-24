#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    
    int exit_state =0;
    int result;
    char *directorio;
    
    if(argc==1){
        printf("remover_directorio: Falta un operando. \n");
        exit_state = 1;
    }else{ 
        directorio = *(argv+1);
        result = rmdir(directorio);
        if(result == -1){
            printf("remover_directorio: El directorio no existe, no se puede eliminar '%s' \n", directorio);
            exit_state = 1;
        }
    }
    
    return exit_state;
}
