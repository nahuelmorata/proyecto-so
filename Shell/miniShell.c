#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>

void ejecutarFuncionEnPoceso(int pid, char dirps[], char *args[]){
  if (pid==0) {

    execvp(dirps,args);
    printf("Error when executing process, try checking the arguments\n");
    exit(EXIT_FAILURE);
  }else {
    if(pid<0){
    perror("Error creating proceso");
    exit(EXIT_FAILURE);
    }
  }
}

char *encontrarRutaComando(char *entrada) {

  if(strcmp(entrada,"minils")==0)
    return "./minils";
  if(strcmp(entrada,"minichmod")==0)
    return "./minils";
  if(strcmp(entrada,"minirmdir")==0)
    return "./minirmdir";
  if(strcmp(entrada,"minitouch")==0)
    return "./minitouch";
  if (strcmp(entrada, "minicat") == 0)
    return "./minicat";
  if (strcmp(entrada, "minimkdir") == 0)
    return "./minimkdir";


  return NULL;
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
        perror("Error getting user's name");
        return ;
    }


    printf("[%s@%s %s]$ ", pw->pw_name, hostname, lastWord);
}


void getHelp(){
  printf("help muestra esta ayuda \n");
  printf("\n");
  printf("minimkdir [nombre_dir] crear un directorio \n");
  printf("\n");
  printf("minirmdir [nombre_dir] <- falta implementar pero elimina un directorio \n");
  printf("\n");
  printf("minitouch [nombre_dir] crea un archivo \n");
  printf("\n");
  printf("minils lista los contenidos del directorio actual \n");
  printf("\n");
  printf("minicat [nombre_archivo] muestra los contenidos de un archivo \n");
  printf("\n");
  printf("minichmod -[params] [nombre_archivo] cambia los permisos de un archivo,\n los permisos son -r para no poder leer +r para leer,\n -w para quitar permisos de escritura +w para dar permisos de escritura\n y -x para quitar permisos de ejecucion y +x para dar permisos de ejecucion\n");
  printf("\n");
  printf("exit para salir de la consola\n");

}

int main() {
//getHelp();
  int salir=0;
  int seguir=1;
  char *palabras[100]; //palabras de max 100 caracteres, despues se cambia si se necesita
  pid_t pid;

  char salida[100];
  printf("iniciando mini shell escriba help para conocer los comandos disponibles o exit para salir\n");
  while (seguir){
    siguienteComando();
    fgets(salida,100,stdin);
    fflush(stdin);

    strtok(salida,"\n");
    salir= strcmp(salida,"exit");
    
    if(salir==0){
      uid_t uid = getuid();
      struct passwd *pw = getpwuid(uid);
      if (pw == NULL) {
        perror("Error getting user's name");
          return 2 ;
      }
      printf("Saliendo de mini Shell, bye %s\n",pw->pw_name);
      break;
    }

    int ayuda = strcmp(salida, "help");
    if(ayuda==0){
      getHelp();
      continue;
    }

    construirArgumentos(salida, palabras);
    pid = fork();
    if((int)pid<=0){
      char * archivoAEjecutar=encontrarRutaComando(palabras[0]);
      if(archivoAEjecutar!=NULL)
      {
      ejecutarFuncionEnPoceso(pid, archivoAEjecutar,palabras);
      }else {
          printf("Comando no reconocido, pruebe escribiendo help para ver los comandos disponibles\n");
          exit(0);
      }
    }
    wait(&pid);

  }



  return EXIT_SUCCESS;
}


