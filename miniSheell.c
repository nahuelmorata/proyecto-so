#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <wait.h>

void ejecutarFuncionEnPoceso(int pid, char dirps[], char *args[]){
  if (pid==0) {
    execvp(dirps,args);
    printf("Error when executing process, try checking the arguments\n");
  }else {
    if(pid<0){
    perror("Error al crear proceso");
    return ;
    }
  }
}

void construirArgumentos(char entrada[], char *argumentosSalida[]){
  const char separator[4] = " ";   
  char * restoString= entrada;
  char *stringSeparado;

  int i=0;
  while ((stringSeparado= strtok_r(restoString,separator,&restoString))!=0) {
    argumentosSalida[i]=stringSeparado;
    i++;
  }
  argumentosSalida[i]=NULL;

}

void getLastWordOfCwd( char * cwd, char last[]){
  int i=0;
  while(*cwd !='\0'){
    i++;
    cwd++;
  }
  int j=i;
  while(*cwd != '/'){
      cwd--;
      j--;
  }
 
  j++;
  cwd++;
  int k=0;
  while(j<i){
    last[k]=*cwd;
    cwd++;
    j++;
    k++;
  }
  
  
}

void siguienteComando(){

 char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) == NULL) {
       perror("getcwd() error");
       return ;
   }

  char lastWord[1024]="\0";
  getLastWordOfCwd(cwd,lastWord);


  char hostname[1024];
  if(  gethostname(hostname, sizeof(hostname)) == -1){
    perror("gethostname error");
    return ;
  }
 
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw == NULL) {
        perror("Error al obtener el nombre del usuario");
        return ;
    }


 printf("[%s@%s %s]$ ",pw->pw_name,hostname,lastWord);
 

}
int main()
{




  int salir=0;
  int seguir=1;
  char *palabras[100]; //palabras de max 100 caracteres, despues se cambia si se necesita

  int i=0; 
  while (seguir && i<10) {
    printf("iniciando consola %d\n",i); 
    siguienteComando();
    char salida[1024]="";
    scanf("%[^\n]s",salida);
    salir= strcmp(salida,"exit");
    
    if(salir==0){
      break;
    }
    printf("%s\n",salida);
       
    construirArgumentos(salida,palabras);
    int pid = fork();
    if(pid>0){
      wait(NULL);
      exit(0);
    } else{
      ejecutarFuncionEnPoceso(pid,palabras[0],palabras);
    }
    printf("Esto va luego de ejecutarse el comando\n");
    i++;
    }


  return EXIT_SUCCESS;
}


