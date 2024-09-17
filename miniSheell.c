#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

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
int main(int argc, char *argv[])
{

 


  int salir=0;
  int seguir=1;

  while (seguir) {
  
    siguienteComando();
    char salida[1024]="";
    scanf("%s",salida);
    salir= strcmp(salida,"exit");
    if(salir==0){
      break;
    }
    
  }

  return EXIT_SUCCESS;
}


