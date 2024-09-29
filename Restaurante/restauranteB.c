#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
 #include <fcntl.h>
#include <sys/stat.h>
#include <sys/msg.h>


#define MAXIMO_CLIENTES 10
#define  PEDIDO_OK "OK"
#define SEM_VACIO "vacio"
#define SEM_TURNO "turno"
#define SEM_TURNO_VIP "turno_vip"
#define FILA_CLIENTES "fila_Clientes"
#define FILA_CLIENTES_VIP "fila_Clientes_VIP"
#define CLAVE_ORDENES 1111
#define CLAVE_OK 9999
#define CLAVE_EMPLEADOS 7777

enum comidas{hamburgesa=1,vegano=2,papas=3};
enum tipo_pedido{normal=1,vip=2};

sem_t *vacio;
sem_t *turno;
sem_t *turnoVIP;

struct pedido {
  long type;
  enum comidas comida;
  int nro_pedido;
};

struct recibido {
  long type;
  enum comidas comida_enviada;
};

struct listo {
  long type;
  enum comidas comida_preparada;
};

int longitud_pedido=sizeof( struct pedido) -sizeof(long);
int longitud_recibido=sizeof(struct recibido) -sizeof(long);
int longitud_listo=sizeof(struct listo) -sizeof(long);



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

void cliente() {
  
  
  int ordenesID=msgget(CLAVE_ORDENES,0666);
  int okID=msgget(CLAVE_OK,0666);
  int myPID=getpid();
  int listoID=msgget(myPID,IPC_CREAT|0666);

  if(ordenesID<0 || okID<0 || listoID<0)
  {
    printf("Error creando cola listo de cliente %d",myPID);
    exit(EXIT_FAILURE);
  }

  struct pedido miPedido;
  enum tipo_pedido mi_tipo=normal;
  miPedido.type=mi_tipo;
  miPedido.comida=obtener_comida();
  miPedido.nro_pedido = myPID;

  printf("Deberia dejar de venir a comer aca\n");
  while (1) {
    if (sem_trywait(vacio)) {
        printf("Entre al restaurante\n");
        sem_wait(turno);
           int res=msgsnd(ordenesID,&miPedido , longitud_pedido,0);
            if(res<0){
              perror("Error mandando pedido");
              exit(EXIT_FAILURE);
            }
           printf("orden pedida de cliente %d y  valor %d\n",myPID,miPedido.comida);
           sleep(1);
           struct recibido pedido_reicbido;
           res=msgrcv(okID,&pedido_reicbido, longitud_recibido,myPID, 0);
           if(res<0){
             perror("Error reciebndo comfirmacion");
             exit(EXIT_FAILURE);
           }
           printf("orden pedida correctamente\n");
          sem_post(turno);
          struct listo mi_pedido_listo;
          res=msgrcv(listoID,&mi_pedido_listo, longitud_listo,0, 0);

          if(res<0){
            perror("Error recibiendo comida");
            exit(EXIT_FAILURE);
          }

          printf("Cliente:la comida fue recibida, no es rica pero bueno\n");


     sem_post(vacio);
     break;
  } else {
        sleep(2);
        printf("Cliente: Me canse me vuelvo mas tarde\n");
     }
  }

exit(0);
}


void cliente_VIP(){
  int ordenesID=msgget(CLAVE_ORDENES,0666);
  int okID=msgget(CLAVE_OK,0666);
  int myPID=getpid();
  int listoID=msgget(myPID,IPC_CREAT|0666);

  if(ordenesID<0 || okID<0 || listoID<0)
  {
    printf("Error creando cola listo vip de cliente %d",myPID);
    exit(EXIT_FAILURE);
  }

  struct pedido miPedido;
  enum tipo_pedido mi_tipo=vip;
  miPedido.type=mi_tipo;
  miPedido.comida=obtener_comida();
  miPedido.nro_pedido=myPID;

  printf("Soy VIP pero deberia dejar de venir a comer aca\n");

  while (1) {
     if (sem_trywait(vacio)) {
          printf("Entre al restaurante\n");
        sem_wait(turnoVIP);

          int res= msgsnd(ordenesID, &miPedido, longitud_pedido, 0);
          if(res<0){
              perror("Error mandando pedido vip");
              exit(EXIT_FAILURE);
            }
            printf("orden VIP pedida de cliente %d y  valor %d\n",myPID,miPedido.comida);
           sleep(1);
          struct recibido pedido_reicbido;
          res=msgrcv(okID,&pedido_reicbido, longitud_recibido,myPID, 0);
         if(res<0){
             perror("Error reciebndo comfirmacion");
             exit(EXIT_FAILURE);
           }
          printf("orden VIP pedida correctamente\n");
          sem_post(turnoVIP);

          struct listo mi_pedido_listo;
          res = msgrcv(listoID, &mi_pedido_listo, longitud_listo, 0, 0);
          if(res<0){
            perror("Error recibiendo comida");
            exit(EXIT_FAILURE);
          }

          printf("VIP:La comida de aca es malisima sin embargo no puedo parar de venir, lo peor es que me hice VIP\n");
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
  int idOrdenes,idOk,idEmpleados;
  idOrdenes=msgget(CLAVE_ORDENES,0666);
  idOk=msgget(CLAVE_OK,0666);
  idEmpleados=msgget(CLAVE_EMPLEADOS,0666);

  if (idEmpleados <0 || idOk<0 || idOrdenes<0)
  {
    perror("Error creando colas de mensajes");
    exit(EXIT_FAILURE);
  }
  struct pedido siguiente_pedido;
  enum comidas comida_por_preparar;
  while (1) {
    enum tipo_pedido tipo_pedido=vip;

    int res = msgrcv(idOrdenes, &siguiente_pedido, longitud_pedido, tipo_pedido,
                     IPC_NOWAIT);


    if(res<0){
      tipo_pedido = normal;
      res= msgrcv(idOrdenes,&siguiente_pedido, longitud_pedido,tipo_pedido, IPC_NOWAIT);
    }

    if (res > 0) {
      comida_por_preparar=siguiente_pedido.comida;
      printf("Siguiente pedido es %d al cliente %d\n", comida_por_preparar,siguiente_pedido.nro_pedido);

    siguiente_pedido.type=comida_por_preparar;
     res=msgsnd(idEmpleados,&siguiente_pedido ,longitud_pedido ,0 );
     if(res<0){
       perror("error enviando orden a empleado");
       exit(EXIT_FAILURE);
    }

    struct recibido pedido_enviado;
    pedido_enviado.type=siguiente_pedido.nro_pedido;
    pedido_enviado.comida_enviada=siguiente_pedido.comida;
    res= msgsnd(idOk, &pedido_enviado, longitud_recibido, 0);
    if(res<0){
      perror("Fallor al enviar confirmacion");
      exit(EXIT_FAILURE);
    }
    /*
     * Podria haber esperado por una comfirmaciond del empleado para ver si no lo estoy pasando de ordenes pero bueno creo que ese nivel
     * de verificacion ya es demasiado
     */
    sleep(3);
  }
  printf("Ordenes:No me gusta atencion al cliente mejor me voy a dormir\n") ;
  sleep(1);

  }
}





void empleado_hamburgesas(){
  int idEmpleado = msgget(CLAVE_EMPLEADOS, 0666);

  if(idEmpleado<0){
          perror("Error obteniendo cola de empleado hamburgesa");
  }
  enum comidas cocinar=hamburgesa;
      while (1) {
        struct pedido siguiente_pedido;
        int res=msgrcv(idEmpleado,&siguiente_pedido ,longitud_pedido ,cocinar ,0 );

        if(res<0){
          perror("Error obteniendo hamburguesa");
          exit(EXIT_FAILURE);
        }
        int idListo=msgget(siguiente_pedido.nro_pedido,0666 );
        printf("comenzando a cocinar hamburguesas\n");
        if(idListo<0){
          perror("Error obteniendo cola del siguiente pedido ");
        }

        struct listo pedido_cocinado;
        pedido_cocinado.comida_preparada=hamburgesa;
        pedido_cocinado.type=siguiente_pedido.nro_pedido;

        res= msgsnd(idListo,&pedido_cocinado ,longitud_listo ,0 );

        if(res<0)
        {
          perror("Error enviando hamburguesa");
          exit(EXIT_FAILURE);
        }

         printf("Se despacho hamburgesa %d en \n",cocinar);
        sleep(1);

        printf("Hamburguesas:Esto de laburar en comida rapida es temporal\n");
        sleep(2);
    } 

}
void empleado_vegano(){

 int idEmpleado=msgget(CLAVE_EMPLEADOS,0666);
 if(idEmpleado<0){
          perror("Error obteniendo cola de empleado vegano");
  }
  enum comidas cocinar=vegano;
      while (1) {

        struct pedido siguiente_pedido;
        int res=msgrcv(idEmpleado,&siguiente_pedido ,longitud_pedido ,cocinar ,0 );

        if(res<0){
          perror("Error obteniendo vegano");
          exit(EXIT_FAILURE);
        }
        int idListo=msgget(siguiente_pedido.nro_pedido,0666 );
        printf("comenzando a cocinar vegano\n");
        if(idListo<0){
          perror("Error obteniendo cola del siguiente pedido ");
        }

        struct listo pedido_cocinado;
        pedido_cocinado.comida_preparada=vegano;
        pedido_cocinado.type=siguiente_pedido.nro_pedido;

        res= msgsnd(idListo,&pedido_cocinado ,longitud_listo ,0 );

        if(res<0)
        {
          perror("Error enviando veganos");
          exit(EXIT_FAILURE);
        }
                sleep(1);

        printf("Vegano:Esto de laburar en comida rapida es temporal\n");
        sleep(3);
    } 
  }





  void empleado_papas(int nro_empleado) {

 int idEmpleado=msgget(CLAVE_EMPLEADOS,0666);
 if(idEmpleado<0){
          perror("Error obteniendo cola de empleado papas");
  }
  enum comidas cocinar=papas;
  while (1) {


        struct pedido siguiente_pedido;
        int res=msgrcv(idEmpleado,&siguiente_pedido ,longitud_pedido ,cocinar ,0 );

        if(res<0){
          perror("Error obteniendo papas");
          exit(EXIT_FAILURE);
        }

        int idListo=msgget(siguiente_pedido.nro_pedido,0666 );
        printf("comenzando a cocinar vegano\n");
        if(idListo<0){
          perror("Error obteniendo cola del siguiente pedido ");
        }

        struct listo pedido_cocinado;
        pedido_cocinado.comida_preparada=vegano;
        pedido_cocinado.type=siguiente_pedido.nro_pedido;

        res= msgsnd(idListo,&pedido_cocinado ,longitud_listo ,0 );

        if(res<0)
        {
          perror("Error enviando veganos");
          exit(EXIT_FAILURE);
        }
        sleep(1);


        printf("Se despacharon papas por el empleado %d\n",nro_empleado);

        sleep(1);

        printf("Papas %d:Esto de laburar en comida rapida es temporal\n",nro_empleado);
        sleep(4);
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
  for (int i=0;i<total_clientes;i++) {
          sleep(1);
          clientes_totales[i]=fork();
          if(clientes_totales[i]==0){
            if ((i%5)==0){
              printf("Cliente VIP %d creado\n",i);
              cliente_VIP();

            }else {
              printf("Cliente %d creado\n",i);
              cliente();
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

  int idOrdenes,idOk,idEmpleados;
  idOrdenes=msgget(CLAVE_ORDENES,IPC_CREAT|0666);
  idOk=msgget(CLAVE_OK,IPC_CREAT|0666);
  idEmpleados=msgget(CLAVE_EMPLEADOS,IPC_CREAT|0666);

  if(idEmpleados<0 || idOk<0 || idOrdenes<0){
    printf("Error creando colas de mensajes %d id Empleados, %d idOk, %d idOrdenes\n",idEmpleados,idOk,idOrdenes);
    exit(EXIT_FAILURE);
  }

int pid=fork();

    if(pid==0){
      crear_laburantes();
      sleep(5);
      exit(0);
    }else {
      if(pid>0){
          vacio= sem_open(SEM_VACIO,O_CREAT | O_EXCL,0644,MAXIMO_CLIENTES);

          turno=sem_open(SEM_TURNO,O_CREAT | O_EXCL,0644,1);
          turnoVIP = sem_open(SEM_TURNO_VIP, O_CREAT | O_EXCL, 0644, 1);

          parte_clientes();

          sem_close(vacio);

          sem_close(turno);
          sem_close(turnoVIP);

          sem_unlink(SEM_VACIO);
          sem_unlink(SEM_TURNO);
          sem_unlink(SEM_TURNO_VIP);

          wait(&pid);
      } else {
        perror("Error creando clientes\n");
        exit(EXIT_FAILURE);
      }

    }



    return EXIT_SUCCESS;
}


