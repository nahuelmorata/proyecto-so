#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


#define MAXIMO_CLIENTES 10
#define PEDIDO_OK "OK"


enum comidas { sin_comida = -1, hamburgesa = 1, vegano = 2, papas = 3 };


int ok_pedido[2];
int hay_pedido[2];
int turno[2];
int turnoVIP[2];
int max_gente[2];

struct comunicacion_empleados {
  int comida_pedida[2];
} emp_papas, emp_hamburgesas, emp_vegano, cliente_ordenes, cliente_ordenes_VIP,
preparado_clientes[3];

struct pedido{
  enum comidas comida;
  int numeroOrden;

};

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

void cliente(int nro_creado) {
  int enojoEspera=0;
  int señal_pedido=1;
  enum comidas comida=obtener_comida();
  struct pedido miOrden;
  int myPID=getpid();
  miOrden.comida=comida;
  miOrden.numeroOrden = myPID;

  int mi_turno,cant;
  struct pedido miComidaLista;
  printf("Deberia dejar de venir a comer aca\n");
  char *ok="";
  while (1) {
    if( read(max_gente[0],&cant ,sizeof(int))>0){
      printf("Entre al restaurante soy el %d\n",nro_creado);

      read(turno[0],&mi_turno ,sizeof(int));

      //   printf("Valor cliente es %d\n",myPID);
      write(cliente_ordenes.comida_pedida[1],&miOrden,sizeof(struct pedido));

      write(hay_pedido[1],&señal_pedido ,sizeof(int) );
      printf("orden pedida de cliente %d y  valor %d\n",myPID,miOrden.comida);

      read(ok_pedido[0],&ok,strlen(PEDIDO_OK));
      write(turno[1],&mi_turno ,sizeof(int));
      // sleep(1);
      printf("orden pedida correctamente\n");
      read(preparado_clientes[comida-1].comida_pedida[0],&miComidaLista,sizeof(struct pedido));

      printf("Cliente:la comida fue recibida, no es rica pero bueno\n");


      write(max_gente[1],&cant,sizeof(int));
      printf("Deje mi espacio en el restaurante soy el %d\n",nro_creado);
      exit(0);
    }else {
      printf("el restaurante esta lleno vuelvo mas tarde\n");
      usleep(100);
      enojoEspera++;
      if (enojoEspera==100){
        printf("Me harte me voy\n");
        break;
      }

    }
  }

}





void cliente_VIP(int nro_creado){
  int enojoEspera=0;
  int señal_pedido=1;
  enum comidas comida = obtener_comida();


  struct pedido miOrden;
  int myPID=getpid();
  miOrden.comida=comida;
  miOrden.numeroOrden = myPID;



  int mi_turno,cant;

  struct pedido miComidaLista;
  printf("Soy VIP pero deberia dejar de venir a comer aca\n");
  char *ok="";
  while (1) {
    if(read(max_gente[0],&cant ,sizeof(int)) >0){
      printf("Entre al restaurante soy el vip %d\n",nro_creado);

      read(turnoVIP[0],&mi_turno ,sizeof(int));
      write(cliente_ordenes_VIP.comida_pedida[1], &miOrden, sizeof(struct pedido));

      write(hay_pedido[1],&señal_pedido ,sizeof(int) );
      //  printf("Valor cliente es %d\n",myPID);
      write(turnoVIP[1],&mi_turno ,sizeof(int));
      printf("orden VIP pedida de cliente %d y  valor %d\n",myPID,miOrden.comida);

      read(ok_pedido[0],&ok,strlen(PEDIDO_OK));
      // sleep(1);
      printf("orden VIP pedida correctamente\n");


      read(preparado_clientes[comida-1].comida_pedida[0],&miComidaLista,sizeof(struct pedido));
      printf("VIP:La comida de aca es malisima sin embargo no puedo parar de venir, lo peor es que me hice VIP\n");

      printf("Deje mi espacio en el restaurante soy el %d\n",nro_creado);
      write(max_gente[1],&cant,sizeof(int));
      exit(0);

    }else {
      printf("Soy vip pero el restaurante esta lleno vuelvo mas tarde\n");
      usleep(100);
      enojoEspera++;
      if (enojoEspera==100){
        printf("Me harte me voy\n");
        break;
      }
    }

  }



}

void empleado_ordenes(){
  enum comidas comida_por_preparar;
  close(hay_pedido[1]);
  close(emp_papas.comida_pedida[0]);
  close(emp_vegano.comida_pedida[0]);
  close(emp_hamburgesas.comida_pedida[0]);

  close(cliente_ordenes.comida_pedida[1]);

  int nroOrden=-1;
  comida_por_preparar = sin_comida;
  close(ok_pedido[0]);

  for (int i=0;i<3 ;i++ ) {
    close(preparado_clientes[i].comida_pedida[0]);
  }


  int hay_pedidos;
  struct pedido nuevo_pedido;
  while (1) {

    read(hay_pedido[0],&hay_pedidos,sizeof(int));
    if(read(cliente_ordenes_VIP.comida_pedida[0], &nuevo_pedido, sizeof(struct pedido))>0){
      printf("Es pedido VIP\n");
    } else {
      if(read(cliente_ordenes.comida_pedida[0], &nuevo_pedido, sizeof(struct pedido))>0)
        printf("Es normal\n");
    }

    nroOrden=nuevo_pedido.numeroOrden;
    comida_por_preparar=nuevo_pedido.comida;

    printf("Siguiente pedido es %d al cliente %d\n",comida_por_preparar,nroOrden);
    if(hamburgesa==comida_por_preparar  ){
      //    printf("comida pedida al empleado hamburgesa\n") ;
      write(emp_hamburgesas.comida_pedida[1], &nuevo_pedido,
            sizeof(struct pedido));

    }
    if(comida_por_preparar==vegano  ){
      //  printf("comida pedida al empleado vegano\n") ;
      write(emp_vegano.comida_pedida[1],&nuevo_pedido,sizeof(struct pedido));

    }
    if(comida_por_preparar==papas  ){
      //   printf("comida pedida al empleado papas\n") ;
      write(emp_papas.comida_pedida[1],&nuevo_pedido,sizeof(struct pedido));

    }

      write(ok_pedido[1], PEDIDO_OK, sizeof(PEDIDO_OK));


    /*
     * Podria haber esperado por una confirmacion del empleado para ver si no lo estoy pasando de ordenes de mas, pero bueno, creo que ese nivel
     * de verificacion ya es demasiado
     */
    nroOrden=-1;
    comida_por_preparar=sin_comida;

    printf("Ordenes:No me gusta atencion al cliente mejor me voy a dormir\n") ;
    // sleep(5);

  }

}



void empleado_hamburgesas() {

  struct pedido nuevo_pedido;

  close(emp_hamburgesas.comida_pedida[1]);

  while (1) {
    read(emp_hamburgesas.comida_pedida[0],&nuevo_pedido,sizeof(struct pedido));

    //     printf("comenzando a cocinar hamburguesas\n");


    write(preparado_clientes[hamburgesa-1].comida_pedida[1], &nuevo_pedido, sizeof(struct pedido));



    printf("Se despacho hamburgesa %d en %d\n",nuevo_pedido.comida,nuevo_pedido.numeroOrden);

    // sleep(1);

    printf("Hamburguesas:Esto de laburar en comida rapida es temporal\n");
    // sleep(2);

  }
}
void empleado_vegano() {

  struct pedido nuevo_pedido;

  close(emp_vegano.comida_pedida[1]);

  while (1) {
    read(emp_vegano.comida_pedida[0],&nuevo_pedido,sizeof(struct pedido));

    //   printf("comenzando a cocinar vegano\n");
    write(preparado_clientes[vegano - 1].comida_pedida[1], &nuevo_pedido,
          sizeof(struct pedido));

    // sleep(1);

    printf("Vegano:Esto de laburar en comida rapida es temporal\n");
    // sleep(3);
  }


}


void empleado_papas(int nro_empleado){

  close(emp_papas.comida_pedida[1]);
  struct pedido nuevo_pedido;

  while (1) {
    read(emp_papas.comida_pedida[0],&nuevo_pedido,sizeof(struct pedido));

    //   printf("comenzando a cocinar papas\n");
    write(preparado_clientes[papas-1].comida_pedida[1],&nuevo_pedido,sizeof(struct pedido));

    printf("Se despacharon papas %d en %d por el empleado %d\n",nuevo_pedido.comida,nuevo_pedido.numeroOrden,nro_empleado);
    //
    // sleep(1);

    printf("Papas %d:Esto de laburar en comida rapida es temporal\n",nro_empleado);
    // sleep(4);

  }
}


void crear_pipes(){

  int resultados_pipe;

  resultados_pipe=pipe(turno);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

  resultados_pipe=pipe(turnoVIP);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

  resultados_pipe=pipe(max_gente);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }
  if (fcntl(max_gente[0], F_SETFL, O_NONBLOCK) < 0)
    exit(2);

  resultados_pipe=pipe(emp_papas.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }



  resultados_pipe=pipe(emp_vegano.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }



  resultados_pipe=pipe(emp_hamburgesas.comida_pedida);
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

  resultados_pipe=pipe(cliente_ordenes_VIP.comida_pedida);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }

  if (fcntl(cliente_ordenes_VIP.comida_pedida[0], F_SETFL, O_NONBLOCK) < 0)
    exit(2);



  resultados_pipe=pipe(ok_pedido);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }


  resultados_pipe=pipe(hay_pedido);
  if(resultados_pipe<0){
    perror("Error creando pipe");
    exit(EXIT_FAILURE);
  }


  for (int i=0;i<3 ;i++ ) {


    resultados_pipe=pipe(preparado_clientes[i].comida_pedida);
    if(resultados_pipe<0){
      perror("Error creando pipe");
      exit(EXIT_FAILURE);
    }

  }

}


void crear_laburantes(){
  close(turno[0]);
  close(turno[1]);
  close(turnoVIP[0]);
  close(turnoVIP[1]);
  close(max_gente[0]);
  close(max_gente[1]);
  // sleep(1);
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
  close(hay_pedido[0]);

  for (int i=0;i<3 ;i++ ) {


    close(preparado_clientes[i].comida_pedida[1]);
  }


  close(cliente_ordenes_VIP.comida_pedida[0]);


  int inicio_gente=10;
  for (int i=0;i<MAXIMO_CLIENTES ;i++ ) {
    write(max_gente[1],&inicio_gente , sizeof(int));
  }
  write(turno[1],&inicio_gente ,sizeof(int) );
  write(turnoVIP[1],&inicio_gente ,sizeof(int) );

  for (int i=0;i<total_clientes;i++) {
    // sleep(1);
    clientes_totales[i]=fork();
    if(clientes_totales[i]==0){
      if ((i%5)==0){
        printf("Cliente VIP %d creado\n",i);
        cliente_VIP(i);
      }else {
        printf("Cliente %d creado\n",i);
        cliente(i);
      }
    }else {
      if(clientes_totales[i]<0){
        perror("Error creando cliente");
        exit(2);
      }
    }
  }




}

void parte_clientes() {
  int total_clientes = MAXIMO_CLIENTES*MAXIMO_CLIENTES;
  int clientes_totales[total_clientes];
  crear_clientes_esperar(total_clientes, clientes_totales);
  // sleep(2);
  for (int i=0;i<total_clientes;i++ ) {
    wait(&clientes_totales[i]);
    printf("Cantidad clientes atendidos = %d\n",(i+1));
  }
  printf("====================================================Todos los clientes satisfechos============================================================\n");
}


int main() {


  crear_pipes();

  int pid=fork();

  if(pid==0){
    crear_laburantes();
    // sleep(5);
    exit(0);
  }else {
    if (pid > 0) {


      parte_clientes();


      wait(&pid);
    } else {
      perror("Error creando clientes\n");
      exit(EXIT_FAILURE);
    }

  }



  // sem_destroy(&vacio);
  return EXIT_SUCCESS;
}


