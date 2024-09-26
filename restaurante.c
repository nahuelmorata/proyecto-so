#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
 #include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


#define MAXIMO_CLIENTES 10
#define  PEDIDO_OK "OK"
#define SEM_VACIO "vacio"
#define SEM_TURNO "turno"
#define SEM_TURNO_VIP "turno_vip"
#define FILA_CLIENTES "fila_Clientes"
#define FILA_CLIENTES_VIP "fila_Clientes_VIP"


enum comidas{sin_comida=-1,hamburgesa=1,hamburgesa_VIP=11,vegano=2,vegano_VIP=22,papas=3,papas_VIP=33};
sem_t *vacio;
sem_t *turno;
sem_t *turnoVIP;
sem_t *fila_Clientes;
sem_t *fila_Clientes_VIP;
int ok_pedido[2];
int ok_pedido_VIP[2];
enum comidas ordenes_clientes[MAXIMO_CLIENTES];
enum comidas comida_preparada_clientes[MAXIMO_CLIENTES];
enum comidas comida_preparada_VIPs[MAXIMO_CLIENTES];
enum comidas ordenes_clientes_VIP[MAXIMO_CLIENTES];

struct comunicacion_empleados {
    int comida_pedida[2];
    int numeroOrden[2];
}emp_papas,emp_hamburgesas,emp_vegano,cliente_ordenes,cliente_ordenes_VIP,preparado_clientes[MAXIMO_CLIENTES],preparado_clientes_VIP[MAXIMO_CLIENTES];

enum comidas obtener_comida(){
int pedido=rand()%6;
  enum comidas comida;
      switch (pedido) {
        case 0:comida=hamburgesa;
          break;
        case 1:comida=vegano;
          break;    
      default: comida=papas;
        break;
      }

  return comida;
}

void *cliente(void *arg) {
  free(arg);
  enum comidas pedido=obtener_comida();
  int miNumero;
  int numero_comida_preparada=-1;
  enum comidas comida_preparada = sin_comida;

  printf("Deberia dejar de venir a comer aca\n");
  while (1) {
    if (sem_trywait(vacio)) {
        printf("Entre al restaurante\n");
        sem_wait(turno);
           sem_getvalue(fila_Clientes,&miNumero);
           sem_post(fila_Clientes);
           miNumero=miNumero%MAXIMO_CLIENTES;
            write(cliente_ordenes.comida_pedida[1],&pedido,sizeof(pedido));
            write(cliente_ordenes.numeroOrden[1],&miNumero,sizeof(int));
           printf("orden pedida de cliente %d y  valor %d\n",miNumero,pedido);
            char *ok="";
            read(ok_pedido[0],&ok,strlen(PEDIDO_OK));
              sleep(1);

           printf("orden pedida correctamente\n");
          sem_post(turno);
          read(preparado_clientes[miNumero].comida_pedida[0],&comida_preparada,sizeof(int));
          read(preparado_clientes[miNumero].numeroOrden[0],&numero_comida_preparada,sizeof(int));
      
        sem_wait(turno);
          printf("Cliente:la comida fue recibida, no es rica pero bueno\n");
            sem_wait(fila_Clientes);
        sem_post(turno);


     sem_post(vacio);
     break;
  } else {
        sleep(2);
        printf("Cliente: Me canse me vuelvo mas tarde\n");
     }
  }

exit(0);
}




enum comidas obtener_comida_VIP(){
int pedido=rand()%6;
  enum comidas comida;
      switch (pedido) {
        case 0:comida=hamburgesa_VIP;
          break;
        case 1:comida=vegano_VIP;
          break;    
        default:comida=papas_VIP;
          break;
      }

  return comida;
}
void *cliente_VIP(void *arg){
   free(arg);

  enum comidas pedido=obtener_comida_VIP();
  int miNumero;
  int numero_comida_preparada=-1;
  enum comidas comida_preparada=sin_comida;
  printf("Soy VIP pero deberia dejar de venir a comer aca\n");
  while (1) {
     if (sem_trywait(vacio)) {
          printf("Entre al restaurante");
        sem_wait(turnoVIP);
           sem_getvalue(fila_Clientes_VIP,&miNumero);
           sem_post(fila_Clientes_VIP);
           miNumero=miNumero%MAXIMO_CLIENTES;
           write(cliente_ordenes_VIP.comida_pedida[1],&pedido,sizeof(pedido));
           write(cliente_ordenes_VIP.numeroOrden[1],&miNumero,sizeof(int));
           printf("orden VIP pedida de cliente %d y  valor %d\n",miNumero,pedido);
            char *ok="";
            read(ok_pedido_VIP[0],&ok,strlen(PEDIDO_OK));
            sleep(1);
          printf("orden VIP pedida correctamente\n");
          sem_post(turnoVIP);
          read(preparado_clientes_VIP[miNumero].comida_pedida[0],&comida_preparada,sizeof(int));
          read(preparado_clientes_VIP[miNumero].numeroOrden[0],&numero_comida_preparada,sizeof(int));
      
        sem_wait(turnoVIP);
          printf("VIP:La comida de aca es malisima sin embargo no puedo parar de venir, lo peor es que me hice VIP\n");
          sem_wait(fila_Clientes_VIP);
        sem_post(turnoVIP);
     sem_post(vacio);
     break;
  } else {
        sleep(2);
        printf("VIP:Soy vip y tengro prioridad,me canse me vuelvo mas tarde\n");
     }
  }


  exit(0);
  
}

void empleado_ordenes(){
  enum comidas comida_por_preparar;
  close(emp_papas.comida_pedida[0]);
  close(emp_vegano.comida_pedida[0]);
  close(emp_hamburgesas.comida_pedida[0]);
  close(emp_papas.numeroOrden[0]);
  close(emp_vegano.numeroOrden[0]);
  close(emp_hamburgesas.numeroOrden[0]);
  close(cliente_ordenes.comida_pedida[1]);
  close(cliente_ordenes.numeroOrden[1]);
  int nroOrden=-1;
  comida_por_preparar = sin_comida;
  close(ok_pedido[0]);
  close(ok_pedido_VIP[0]);
  for (int i=0;i<MAXIMO_CLIENTES;i++) {
      close(preparado_clientes[i].comida_pedida[0]);
      close(preparado_clientes[i].numeroOrden[0]);
    }
int isVIP=0;
  while (1) {

    if(read(cliente_ordenes_VIP.comida_pedida[0], &comida_por_preparar, sizeof(int))>0){
      read(cliente_ordenes_VIP.numeroOrden[0],&nroOrden,sizeof(int));
      isVIP++;
    } else {
      if(read(cliente_ordenes.comida_pedida[0], &comida_por_preparar, sizeof(int))>0)
              read(cliente_ordenes.numeroOrden[0],&nroOrden,sizeof(int));
    }
    if (comida_por_preparar > (sin_comida) && nroOrden>=0) {
      printf("Siguiente pedido es %d al cliente %d\n",comida_por_preparar,nroOrden);
      if(hamburgesa==comida_por_preparar || comida_por_preparar==hamburgesa_VIP){
          printf("comida pedida al empleado hamburgesa\n") ;
         write(emp_hamburgesas.comida_pedida[1],&comida_por_preparar,sizeof(int));
         write(emp_hamburgesas.numeroOrden[1],&nroOrden,sizeof(int));
      }
      if(comida_por_preparar==vegano || comida_por_preparar==vegano_VIP){
      printf("comida pedida al empleado vegano\n") ;
       write(emp_vegano.comida_pedida[1],&comida_por_preparar,sizeof(int));
      write(emp_vegano.numeroOrden[1],&nroOrden,sizeof(int)); 
    }
    if(comida_por_preparar==papas || comida_por_preparar==papas_VIP){
      printf("comida pedida al empleado papas\n") ;
      write(emp_papas.comida_pedida[1],&comida_por_preparar,sizeof(int));
      write(emp_papas.numeroOrden[1],&nroOrden,sizeof(int));
    }
    if(isVIP==1){
      write(ok_pedido_VIP[1], PEDIDO_OK, sizeof(PEDIDO_OK));
        isVIP=0;
    }else {
      write(ok_pedido[1], PEDIDO_OK, sizeof(PEDIDO_OK));
    }
    nroOrden=-1;
    comida_por_preparar=sin_comida;
    }else{
      printf("Ordenes:No me gusta atencion al cliente mejor me voy a dormir\n") ;
      sleep(5);
    }
  }

}





void empleado_hamburgesas(){
  enum comidas cocinar;
  int numeroEnfila;
  close(emp_hamburgesas.comida_pedida[1]);
   close(emp_hamburgesas.numeroOrden[1]);
      while (1) {
        read(emp_hamburgesas.comida_pedida[0],&cocinar,sizeof(int));
        if(cocinar==hamburgesa || cocinar==hamburgesa_VIP){
          printf("comenzando a cocinar hamburguesas\n");
          read(emp_hamburgesas.numeroOrden[0],&numeroEnfila,sizeof(int));
  
        if(cocinar==hamburgesa_VIP){
            write(preparado_clientes_VIP[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes_VIP[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
      }else {
           write(preparado_clientes[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
        }
        
        printf("Se despacho hamburgesa %d en %d\n",cocinar,numeroEnfila);
        cocinar=sin_comida;
        numeroEnfila=-1;
        sleep(1);
    }else {
        printf("Hamburguesas:Esto de laburar en comida rapida es temporal\n");
        sleep(2);
    } 
  }
}
void empleado_vegano(){
 enum comidas cocinar;
  int numeroEnfila;
  close(emp_vegano.comida_pedida[1]);
   close(emp_vegano.numeroOrden[1]);

      while (1) {
        read(emp_vegano.comida_pedida[0],&cocinar,sizeof(int));
        if(cocinar==vegano || cocinar==vegano_VIP){
          printf("comenzando a cocinar vegano\n");
          read(emp_vegano.numeroOrden[0],&numeroEnfila,sizeof(int));
           
        if(cocinar==vegano_VIP){
            write(preparado_clientes_VIP[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes_VIP[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
        printf("Se despacho menu vegano VIP %d en %d\n",cocinar,numeroEnfila);

      }else {
            write(preparado_clientes[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
        }
      cocinar=sin_comida;
      numeroEnfila=-1;
                sleep(1);
    }else {
        printf("Vegano:Esto de laburar en comida rapida es temporal\n");
        sleep(3);
    } 
  }


}


void empleado_papas(int nro_empleado){
  enum comidas cocinar;
  int numeroEnfila;
  close(emp_papas.comida_pedida[1]);
  close(emp_papas.numeroOrden[1]);
  while (1) {
        read(emp_papas.comida_pedida[0],&cocinar,sizeof(int));
        if(cocinar==papas || cocinar==papas_VIP){
          printf("comenzando a cocinar papas\n");
          read(emp_papas.numeroOrden[0],&numeroEnfila,sizeof(int));
  
        if(cocinar==papas_VIP){
            write(preparado_clientes_VIP[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes_VIP[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
      }else {
           write(preparado_clientes[numeroEnfila].comida_pedida[1],&cocinar,sizeof(int));
            write(preparado_clientes[numeroEnfila].numeroOrden[1],&numeroEnfila,sizeof(int));
        }
        printf("Se despacharon papas %d en %d por el empleado %d\n",cocinar,numeroEnfila,nro_empleado);
        cocinar=sin_comida;
        numeroEnfila=-1;
        sleep(1);
    }else {
        printf("Papas %d:Esto de laburar en comida rapida es temporal\n",nro_empleado);
        sleep(4);
    } 
  }
}


void crear_pipes(){

  int resultados_pipe;

  resultados_pipe=pipe(emp_papas.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }
 if (fcntl(emp_papas.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

   resultados_pipe=pipe(emp_papas.numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }
 resultados_pipe=pipe(emp_vegano.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }
 if (fcntl(emp_vegano.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

 resultados_pipe=pipe(emp_vegano.numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }
 resultados_pipe=pipe(emp_hamburgesas.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

  if (fcntl(emp_hamburgesas.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

 resultados_pipe=pipe(emp_hamburgesas.numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

 resultados_pipe=pipe(cliente_ordenes.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

if (fcntl(cliente_ordenes.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

  resultados_pipe=pipe(cliente_ordenes.numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

resultados_pipe=pipe(cliente_ordenes_VIP.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

if (fcntl(cliente_ordenes_VIP.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

 resultados_pipe=pipe(cliente_ordenes_VIP.numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

 resultados_pipe=pipe(ok_pedido);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

resultados_pipe=pipe(ok_pedido_VIP);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < MAXIMO_CLIENTES; i++) {

  resultados_pipe=pipe(preparado_clientes[i].comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

 resultados_pipe=pipe(preparado_clientes[i].numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

resultados_pipe=pipe(preparado_clientes_VIP[i].comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

 resultados_pipe=pipe(preparado_clientes_VIP[i].numeroOrden);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

}
}


void crear_laburantes(){
   sleep(1);
    int pid2=fork();
    if(pid2==0){
     printf("empleado hamburguesas creado\n");
      empleado_hamburgesas();
           exit(0);
    }else{
        if(pid2>0){
          int pid3=fork();
          if(pid3==0){
            printf("empleado papas 1 creado\n");
           empleado_papas(1);
          }else {
            if(pid3>0){
              int pid4=fork();
              if(pid4==0){
                printf("empleado_vegano creado\n");
               empleado_vegano();
              }else {
              if(pid4>0){
                int pid5=fork();
                if(pid5==0){
                   printf("empleado papas 2 creado\n");
                  empleado_papas(2);
                  }else{
                  if(pid5>0){
                     printf("empleado ordenes creado\n");
                    empleado_ordenes();

                  }else {
                perror("Error creando papas");
                exit(EXIT_FAILURE);
                }
                }
              }else {
             perror("Error creando vegano");
             exit(EXIT_FAILURE);
            }
            }
          }else {
        perror("Error creando papas 1");
        exit(EXIT_FAILURE);
      }
        }
      }else {
        perror("Error creando hamburguesas");
        exit(EXIT_FAILURE);
      }
    }
}

void crear_clientes_esperar(int total_clientes,int clientes_totales[]){
   close(ok_pedido[1]);
    close(cliente_ordenes.comida_pedida[0]);
    close(cliente_ordenes.numeroOrden[0]);
    for (int i=0;i<MAXIMO_CLIENTES;i++) {
      close(preparado_clientes[i].comida_pedida[1]);
      close(preparado_clientes[i].numeroOrden[1]);
    }

    close(ok_pedido_VIP[1]);
    close(cliente_ordenes_VIP.comida_pedida[0]);
    close(cliente_ordenes_VIP.numeroOrden[0]);
    for (int i=0;i<MAXIMO_CLIENTES;i++) {
      close(preparado_clientes_VIP[i].comida_pedida[1]);
      close(preparado_clientes_VIP[i].numeroOrden[1]);
    }
  for (int i=0;i<total_clientes;i++) {
          sleep(1);
          clientes_totales[i]=fork();
          if(clientes_totales[i]==0){
            if ((i%5)==0){
              printf("Cliente VIP %d creado\n",i);
              cliente_VIP(NULL);

            }else {
              printf("Cliente %d creado\n",i);
              cliente(NULL);
            }
          }else {
            if(clientes_totales[i]<0){
              perror("Error creando cliente");
              exit(2);
            }
          }
      }

      for (int i=0;i<total_clientes ;i++ ) {
              wait(&clientes_totales[i]);
      }


}

void parte_clientes() {
          int total_clientes = MAXIMO_CLIENTES;
          int clientes_totales[total_clientes];
          crear_clientes_esperar(total_clientes, clientes_totales);
          printf("====================================================Todos los clientes satisfechos============================================================\n");
}


int main() {

  for (int i=0;i<MAXIMO_CLIENTES;i++) {
    ordenes_clientes[i]=sin_comida;
    ordenes_clientes_VIP[i]=sin_comida;
  }
  crear_pipes();

int pid=fork();

    if(pid==0){
      crear_laburantes();
      sleep(5);
      exit(0);
    }else {
      if(pid>0){
          vacio= sem_open(SEM_VACIO,O_CREAT | O_EXCL,0644,MAXIMO_CLIENTES);

          turno=sem_open(SEM_TURNO,O_CREAT | O_EXCL,0644,1);
          turnoVIP=sem_open(SEM_TURNO_VIP,O_CREAT | O_EXCL,0644,1);

          fila_Clientes=sem_open(FILA_CLIENTES,O_CREAT | O_EXCL,0644,1);
          fila_Clientes_VIP =sem_open(FILA_CLIENTES_VIP, O_CREAT | O_EXCL, 0644, 1);

          parte_clientes();

          sem_close(vacio);

          sem_close(turno);
          sem_close(turnoVIP);

          sem_close(fila_Clientes);
          sem_close(fila_Clientes_VIP);

          sem_unlink(SEM_VACIO);
          sem_unlink(SEM_TURNO);
          sem_unlink(SEM_TURNO_VIP);

          sem_unlink(FILA_CLIENTES);
          sem_unlink(FILA_CLIENTES_VIP);

          wait(&pid);
      } else {
        perror("Error creando clientes\n");
        exit(EXIT_FAILURE);
      }

    }



    // sem_destroy(&vacio);
    return EXIT_SUCCESS;
}


