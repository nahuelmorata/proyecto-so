#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#define size 100

void getParams(char buffer[], char *param[]){
    char *t = strtok(buffer," ");
    int i = 0;
    while (t != NULL){
        param[i++] = t;
        t= strtok(NULL, " ");
    }
    param[i] = t;
}

int main(){
    pid_t p;
    char buffer[size], aux[size];
    char *param[size];
    char direccion[size];    
    
    while(1){
        printf("shell>\t");
        fgets(buffer, size, stdin);
        
        strtok(buffer,"\n");      
        strcpy(aux,buffer);
        char *cmd = strtok(aux," ");

        getParams(buffer,param); 

        getcwd(direccion, sizeof(direccion));

        if(strcmp(cmd,"crear_directorio") == 0)   *direccion = *strcat(direccion,"/archivos/ejecutables/crear_directorio");
        if(strcmp(cmd,"remover_directorio") == 0) *direccion = *strcat(direccion,"/archivos/ejecutables/remover_directorio");
        if(strcmp(cmd,"listar_directorio") == 0)  *direccion = *strcat(direccion,"/archivos/ejecutables/listar_directorio");
        if(strcmp(cmd,"crear_archivo") == 0)      *direccion = *strcat(direccion,"/archivos/ejecutables/crear_archivo"); 
        if(strcmp(cmd,"remover_archivo") == 0)    *direccion = *strcat(direccion,"/archivos/ejecutables/remover_archivo");  
        if(strcmp(cmd,"mostrar_archivo") == 0)    *direccion = *strcat(direccion,"/archivos/ejecutables/mostrar_archivo");     
        if(strcmp(cmd,"permisos_archivo") == 0)   *direccion = *strcat(direccion,"/archivos/ejecutables/permisos_archivo");
        if(strcmp(cmd,"help") == 0){
            *direccion = *strcat(direccion,"/archivos/ejecutables/mostrar_archivo");  
            param[1] = "archivos/shell/sh_help.txt";
        }
        if(strcmp(cmd,"exit") == 0)kill(getpid(),SIGKILL);

        p = fork();
        if((int)p==0)execl(direccion,direccion,*(param+1),*(param+2),(char*)NULL);
        wait(&p); 
    }
    return 0;
}


