#include <stdio.h>

int main(int argc, char **argv){
	
    int exit_state = 0;
    FILE *archivo;
    char ch;
    
    if(argc==1){
	fprintf(stderr,"mostrar_archivo: No hay argumento. \n");
        exit_state = 1;
    }else{
	archivo = fopen(argv[1], "r");
	
	if(archivo == NULL){
		printf("mostrar_archivo: No existe el archivo %s.\n", argv[1]);
	}else{
		ch = fgetc(archivo);
		while(ch != EOF){
			printf("%c",ch);
			ch = fgetc(archivo);
		}
		printf("\n");
		fclose(archivo);
	}
    }
    
    return exit_state;
}
