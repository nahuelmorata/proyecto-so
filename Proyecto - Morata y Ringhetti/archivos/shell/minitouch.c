#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    
    int exit_state = 0;
    int result;
    char *direccion;
    
    if(argc==1){
        fprintf(stderr,"crear_archivo: No hay argumento. \n");
        exit_state = 1;
    }else{
        direccion = *(argv+1);
        result = open(direccion,O_WRONLY|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if (result == -1){
            fprintf(stderr, "crear_archivo: El archivo %s ya existe. \n", direccion);
            exit_state = 1;
        }
    }
    
    return exit_state;
}

