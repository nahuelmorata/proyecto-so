#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    
    int exit_state = 0;
    int result;
    char *directorio;
    
    if(argc==1){
        printf("crear_directorio: Falta un operando. \n");
        exit_state = 1;
    }else{ 
        directorio = *(argv+1);
        result = mkdir(directorio,0777);
        if(result == -1){
            printf("crear_directorio: El directorio ya existe, no se puede crear '%s'\n", directorio);
            exit_state = 1;
        }
    }
    
    return exit_state;
}
