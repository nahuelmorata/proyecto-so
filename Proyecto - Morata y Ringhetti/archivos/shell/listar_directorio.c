#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv) {
    
    int exit_state = 0;
    DIR *directorio;
    struct dirent *dir;
    char *direcccion;

    if(argc==1){
        direcccion = ".";
    }else{
        direcccion = *(argv+1);
    }
    
    directorio = opendir(direcccion);
    
    if(directorio){
        while((dir = readdir(directorio)) != NULL)printf("%s\n", dir->d_name);
        closedir(directorio);
    }else{
        fprintf(stderr,"listar_directorio: : No existe el archivo o el directorio, no se puede acceder a '%s'. \n", direcccion);
        exit_state = 2;
    }
    
    return exit_state;
}
