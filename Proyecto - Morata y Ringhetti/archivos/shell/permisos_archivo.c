#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char **argv){
    
    int exit_state = 0;
    mode_t permisos;
    
    if(argc<3){
        fprintf(stderr, "permisos_archivo: Operandos incorrectos. \n");
        exit_state = 1;
    }else{
        if(strcmp(*(argv+2),"r")==0)   permisos = S_IRUSR|S_IRGRP|S_IROTH;
        if(strcmp(*(argv+2),"w")==0)   permisos = S_IWUSR|S_IWGRP|S_IWOTH;
        if(strcmp(*(argv+2),"x")==0)   permisos = S_IXUSR|S_IXGRP|S_IXOTH;
        if(strcmp(*(argv+2),"rw")==0)  permisos = S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH;
        if(strcmp(*(argv+2),"rx")==0)  permisos = S_IRUSR|S_IRGRP|S_IROTH|S_IXUSR|S_IXGRP|S_IXOTH;
        if(strcmp(*(argv+2),"wx")==0)  permisos = S_IWUSR|S_IWGRP|S_IWOTH|S_IXUSR|S_IXGRP|S_IXOTH;
        if(strcmp(*(argv+2),"rwx")==0) permisos = S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH|S_IXUSR|S_IXGRP|S_IXOTH;
        
		exit_state = chmod(argv[1],permisos);
        
        if(exit_state==-1)
            printf("permisos_archivo: No se pudieron modificar los permisos.");
	}
    
    return exit_state;
}
