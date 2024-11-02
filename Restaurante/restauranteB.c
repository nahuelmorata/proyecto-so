#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

#define MAXIMO_CLIENTES 10
#define PEDIDO_OK "OK"

#define CLAVE "clientesEnMicolaNsNs"

#define CODIGO_ORDENES 56
#define CODIGO_OK 10
#define CODIGO_EMPLEADOS 43
#define CODIGO_LISTO 34

enum comidas { hamburgesa = 1, vegano = 2, papas = 3 };
enum tipo_pedido { normal = 1, vip = 2, señal_pedido = 3,max_gente=4, turno=5,turnoVIP=6};


struct pedido {
  long type;
  enum comidas comida;
  int nro_pedido;
};

struct recibido {
  long type;
};

struct listo {
  long type;
  enum comidas comida_preparada;
};

int longitud_pedido = sizeof(struct pedido) - sizeof(long);
int longitud_recibido = sizeof(struct recibido) - sizeof(long);
int longitud_listo = sizeof(struct listo) - sizeof(long);

enum comidas obtener_comida() {
  int pedido = rand() % 6;
  enum comidas comida;
  switch (pedido) {
  case 0:
    comida = hamburgesa;
    break;
  case 1:
    comida = vegano;
    break;
  default:
    comida = papas;
    break;
  }

  return comida;
}
key_t obtener_clave(char *frase, int id) {
  key_t res = ftok(frase, id);
  //printf("Se obtuvo clave %d con frase %s e id %d\n",(int) res ,frase , id);
  return res;
}
void cliente() {
  int enojoEspera=0;
  int ordenesID = msgget(obtener_clave(CLAVE, CODIGO_ORDENES), 0666);
  int okID = msgget(obtener_clave(CLAVE, CODIGO_OK), 0666);
  int myPID = getpid();
  int listoID = msgget(obtener_clave(CLAVE, CODIGO_LISTO), 0666);

  if (ordenesID < 0 || okID < 0 || listoID < 0) {
    printf("Error creando cola listo de cliente %d", myPID);
    exit(EXIT_FAILURE);
  }

  struct pedido miPedido;
  enum tipo_pedido mi_tipo = normal;
  miPedido.type = mi_tipo;
  miPedido.comida = obtener_comida();
  miPedido.nro_pedido = myPID;

  printf("Deberia dejar de venir a comer aca\n");
   fflush(stdout);
  struct recibido señal;
struct listo pedido_listo;
  while (1) {

    if (msgrcv(okID,&señal ,longitud_recibido ,max_gente ,IPC_NOWAIT)>=0) {

      printf("Entre al restaurante\n");
      fflush(stdout);
      msgrcv(okID,&señal , longitud_recibido,turno ,0 );

      int res = msgsnd(ordenesID, &miPedido, longitud_pedido, 0);
      if (res < 0) {
        perror("Error mandando pedido");
        exit(EXIT_FAILURE);
      }
      señal.type = señal_pedido;
      res = msgsnd(okID, &señal, longitud_recibido, 0);
      if (res < 0) {
        perror("Error haciendo pedido");
        exit(EXIT_FAILURE);
      }

      printf("orden pedida de cliente %d y  valor %d\n", myPID,
             miPedido.comida);
      fflush(stdout);
       // sleep(1);

      res = msgrcv(okID, &señal, longitud_recibido, myPID, 0);

      if (res < 0) {
        perror("Error reciebndo comfirmacion");
        exit(EXIT_FAILURE);
      }
      printf("orden pedida correctamente\n");
      fflush(stdout);
      señal.type=turno;

      res= msgsnd(okID,&señal , longitud_recibido,0 );
       if (res < 0) {
        perror("Error dejando turno");
        exit(EXIT_FAILURE);
      }


      res = msgrcv(listoID, &pedido_listo, longitud_listo, myPID, 0);

      if (res < 0) {
        perror("Error recibiendo comida");
        exit(EXIT_FAILURE);
      }

      printf("Cliente:la comida fue recibida, no es rica pero bueno\n");

      fflush(stdout);

      señal.type=max_gente;
     res= msgsnd(okID,&señal , longitud_recibido,0 );
      if (res < 0) {
        perror("Error dejando restaurante");
        exit(EXIT_FAILURE);
      }
      break;
    } else {
      printf("Cliente: Me canse me vuelvo mas tarde al restaurante\n");
      fflush(stdout);
      usleep(100);
      enojoEspera++;
      if(enojoEspera==100)
        break;
    }

  }

  printf("Sali del restaurante\n");
  exit(0);
}

void cliente_VIP() {
   int enojoEspera=0;
  int ordenesID = msgget(obtener_clave(CLAVE, CODIGO_ORDENES), 0666);
  int okID = msgget(obtener_clave(CLAVE, CODIGO_OK), 0666);
  int myPID = getpid();
  int listoID = msgget(obtener_clave(CLAVE, CODIGO_LISTO), 0666);

  if (ordenesID < 0 || okID < 0 || listoID < 0) {
    printf("Error creando cola listo vip de cliente %d", myPID);
    exit(EXIT_FAILURE);
  }

  struct pedido miPedido;
  enum tipo_pedido mi_tipo = vip;
  miPedido.type = mi_tipo;
  miPedido.comida = obtener_comida();
  miPedido.nro_pedido = myPID;

  printf("Soy VIP pero deberia dejar de venir a comer aca\n");
  fflush(stdout);
  struct recibido señal;
  struct listo pedido_listo;
  while (1) {
    if (msgrcv(okID,&señal ,longitud_recibido ,max_gente ,IPC_NOWAIT )>=0) {
      printf("Entro VIP al restaurante\n");
      fflush(stdout);
      msgrcv(okID,&señal , longitud_recibido,turnoVIP ,0 );

      int res = msgsnd(ordenesID, &miPedido, longitud_pedido, 0);
      if (res < 0) {
        perror("Error mandando pedido vip");
        exit(EXIT_FAILURE);
      }

      señal.type = señal_pedido;
      res = msgsnd(okID, &señal, longitud_recibido, 0);
      if (res < 0) {
        perror("Error haciendo pedido vip");
        exit(EXIT_FAILURE);
      }
      printf("orden VIP pedida de cliente %d y  valor %d\n", myPID,
             miPedido.comida);
      fflush(stdout);
       // sleep(1);

      res = msgrcv(okID, &señal, longitud_recibido, myPID, 0);

      if (res < 0) {
        perror("Error reciebndo comfirmacion");
        exit(EXIT_FAILURE);
      }
      printf("orden VIP pedida correctamente\n");
      señal.type=turnoVIP;
      res= msgsnd(okID,&señal , longitud_recibido,0 );
      if (res < 0) {
        perror("Error dejando turno");
        exit(EXIT_FAILURE);
      }


      res = msgrcv(listoID, &pedido_listo, longitud_listo, myPID, 0);
      if (res < 0) {
        perror("Error recibiendo comida");
        exit(EXIT_FAILURE);
      }

      printf("VIP:La comida de aca es malisima sin embargo no puedo parar de "
             "venir, lo peor es que me hice VIP\n");

      fflush(stdout);
      señal.type=max_gente;
     res= msgsnd(okID,&señal , longitud_recibido,0 );
        if (res < 0) {
        perror("Error dejando restaurante");
        exit(EXIT_FAILURE);
      }
      break;
    } else {

      printf("VIP:Soy vip y tengro prioridad,me canse me vuelvo mas tarde hay en el restaurante \n");
      fflush(stdout);
      usleep(100);
      enojoEspera++;
      if(enojoEspera==100)
        break;
    }

  }
 printf("Sali del restaurante\n");
  exit(0);
}

void empleado_ordenes() {
  int idOrdenes, idOk, idEmpleados;
  idOrdenes = msgget(obtener_clave(CLAVE, CODIGO_ORDENES), 0666);
  idOk = msgget(obtener_clave(CLAVE, CODIGO_OK), 0666);
  idEmpleados = msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), 0666);

  if (idEmpleados < 0 || idOk < 0 || idOrdenes < 0) {
    perror("Error creando colas de mensajes");
    exit(EXIT_FAILURE);
  }
  struct pedido siguiente_pedido;
  enum comidas comida_por_preparar;
  struct recibido señal;
  while (1) {

    int res = msgrcv(idOk, &señal, longitud_recibido, señal_pedido, 0);
    printf("Se recibio un pedido\n");
    fflush(stdout);

    if (res < 0) {
      perror("Error recibiendo pedido\n");
      exit(EXIT_FAILURE);
    }

    enum tipo_pedido tipo_pedido = vip;
    res = msgrcv(idOrdenes, &siguiente_pedido, longitud_pedido, tipo_pedido,
                 IPC_NOWAIT);

    if (res < 0) {
      tipo_pedido = normal;
      res = msgrcv(idOrdenes, &siguiente_pedido, longitud_pedido, tipo_pedido,
                   IPC_NOWAIT);
    }

    printf("Se leyo de la cola de ordenes %d\n",res);
    if (res > 0) {
      comida_por_preparar = siguiente_pedido.comida;
      printf("Siguiente pedido es %d al cliente %d\n", comida_por_preparar, siguiente_pedido.nro_pedido);
      fflush(stdout);

      siguiente_pedido.type = comida_por_preparar;
      res = msgsnd(idEmpleados, &siguiente_pedido, longitud_pedido, 0);
      if (res < 0) {
        perror("error enviando orden a empleado");
        exit(EXIT_FAILURE);
      }

      señal.type = siguiente_pedido.nro_pedido;
      res = msgsnd(idOk, &señal, longitud_recibido, 0);
      if (res < 0) {
        perror("Fallor al enviar confirmacion");
        exit(EXIT_FAILURE);
      }
      printf("Se mando confirmacion a %d\n",siguiente_pedido.nro_pedido);
      /*
       * Podria haber esperado por una comfirmaciond del empleado para ver si no
       * lo estoy pasando de ordenes pero bueno creo que ese nivel de
       * verificacion ya es demasiado
       */
       // sleep(3);
    }
    printf("Ordenes:No me gusta atencion al cliente mejor me voy a dormir\n");
    fflush(stdout);
     // sleep(1);
  }
}

void empleado_hamburgesas() {
  int idEmpleado = msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), 0666);

  if (idEmpleado < 0) {
    perror("Error obteniendo cola de empleado hamburgesa");
  }
  enum comidas cocinar = hamburgesa;
  struct pedido siguiente_pedido;

    int idListo =
        msgget(obtener_clave(CLAVE, CODIGO_LISTO), 0666);
  while (1) {

    int res =
        msgrcv(idEmpleado, &siguiente_pedido, longitud_pedido, cocinar, 0);
    printf("Resultado buscar hamburguesa %d\n", res);
    fflush(stdout);
    if (res < 0) {
      perror("Error obteniendo hamburguesa");
      exit(EXIT_FAILURE);
    }

    printf("comenzando a cocinar hamburguesas\n");
    fflush(stdout);

    struct listo pedido_cocinado;
    pedido_cocinado.comida_preparada = hamburgesa;
    pedido_cocinado.type = siguiente_pedido.nro_pedido;

    if (idListo < 0) {
      perror("Error obteniendo cola del siguiente pedido ");
    }
    res = msgsnd(idListo, &pedido_cocinado, longitud_listo, 0);

    if (res < 0) {
      perror("Error enviando hamburguesa");
      exit(EXIT_FAILURE);
    }

    printf("Se despacho hamburgesa %d en %d\n", cocinar,
           siguiente_pedido.nro_pedido);
    fflush(stdout);
     // sleep(1);

    printf("Hamburguesas:Esto de laburar en comida rapida es temporal\n");
    fflush(stdout);
     // sleep(2);
  }
}
void empleado_vegano() {

  int idEmpleado = msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), 0666);

  if (idEmpleado < 0) {
    perror("Error obteniendo cola de empleado vegano");
  }
   int idListo =
        msgget(obtener_clave(CLAVE, CODIGO_LISTO), 0666);
  enum comidas cocinar = vegano;
  while (1) {

    struct pedido siguiente_pedido;
    int res =
        msgrcv(idEmpleado, &siguiente_pedido, longitud_pedido, cocinar, 0);

    if (res < 0) {
      perror("Error obteniendo vegano");
      exit(EXIT_FAILURE);
    }

    printf("comenzando a cocinar vegano\n");
    fflush(stdout);
    if (idListo < 0) {
      perror("Error obteniendo cola del siguiente pedido ");
    }

    struct listo pedido_cocinado;
    pedido_cocinado.comida_preparada = vegano;
    pedido_cocinado.type = siguiente_pedido.nro_pedido;

    res = msgsnd(idListo, &pedido_cocinado, longitud_listo, 0);
    printf("Se despacho vegano %d en %d\n", cocinar,
           siguiente_pedido.nro_pedido);
    fflush(stdout);


    if (res < 0) {
      perror("Error enviando veganos");
      exit(EXIT_FAILURE);
    }
     // sleep(1);

    printf("Vegano:Esto de laburar en comida rapida es temporal\n");
    fflush(stdout);
     // sleep(3);
  }
}

void empleado_papas(int nro_empleado) {

  int idEmpleado = msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), 0666);

  if (idEmpleado < 0) {
    perror("Error obteniendo cola de empleado papas");
  }
  enum comidas cocinar = papas;

    int idListo =
        msgget(obtener_clave(CLAVE, CODIGO_LISTO), 0666);
  struct pedido siguiente_pedido;
  while (1) {
    int res =
        msgrcv(idEmpleado, &siguiente_pedido, longitud_pedido, cocinar, 0);

    if (res < 0) {
      perror("Error obteniendo papas");
      exit(EXIT_FAILURE);
    }

    printf("comenzando a cocinar papas\n");
    fflush(stdout);
    if (idListo < 0) {
      perror("Error obteniendo cola del siguiente pedido ");
    }

    struct listo pedido_cocinado;
    pedido_cocinado.comida_preparada = papas;
    pedido_cocinado.type = siguiente_pedido.nro_pedido;

    res = msgsnd(idListo, &pedido_cocinado, longitud_listo, 0);
    printf("Se despacho papas %d en %d\n", cocinar,
           siguiente_pedido.nro_pedido);
    fflush(stdout);

    if (res < 0) {
      perror("Error enviando papas");
      exit(EXIT_FAILURE);
    }
     // sleep(1);


    printf("Se despacharon papas por el empleado %d\n", nro_empleado);
    fflush(stdout);

     // sleep(1);

    printf("Papas %d:Esto de laburar en comida rapida es temporal\n",
           nro_empleado);
    fflush(stdout);
     // sleep(4);
  }
}

void crear_laburantes() {
   // sleep(1);

  int pid2 = fork();
  if (pid2 == 0) {
    printf("empleado hamburguesas creado\n");
    empleado_hamburgesas();
    exit(0);
  }
  if (pid2 < 0) {
    perror("Error creando hamburguesas");
    exit(EXIT_FAILURE);
  }

  int pid3 = fork();
  if (pid3 == 0) {
    printf("empleado papas 1 creado\n");
    empleado_papas(1);
  }
  if (pid2 < 0) {
    perror("Error creando papas 1");
    exit(EXIT_FAILURE);
  }

  int pid4 = fork();
  if (pid4 == 0) {
    printf("empleado_vegano creado\n");
    empleado_vegano();
  }
  if (pid2 < 0) {
    perror("Error creando vegano");
    exit(EXIT_FAILURE);
  }
  int pid5 = fork();
  if (pid5 == 0) {
    printf("empleado papas 2 creado\n");
    empleado_papas(2);
  }
  if (pid2 < 0) {
    perror("Error creando papas 2");
    exit(EXIT_FAILURE);
  }
  printf("empleado ordenes creado\n");
  empleado_ordenes();
}

void crear_clientes_esperar(int total_clientes, int clientes_totales[]) {
  for (int i = 0; i < total_clientes; i++) {
     // sleep(1);
    clientes_totales[i] = fork();
    if (clientes_totales[i] == 0) {
      if ((i%5)== 0) {
        printf("Cliente VIP %d creado\n", i);
        fflush(stdout);
        cliente_VIP();

      } else {
        printf("Cliente %d creado\n", i);
        fflush(stdout);
        cliente();
      }
    } else {
      if (clientes_totales[i] < 0) {
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
   // sleep(3);
    for (int i = 0; i < total_clientes; i++) {
    wait(&clientes_totales[i]);
  }
  printf(
      "====================================================Todos los clientes "
      "satisfechos============================================================"
      "\n");
}

int main() {

  int idOrdenes, idOk, idEmpleados,idListo;
  printf("\n=============Abriendo Pumper Nic=================\n");
  fflush(stdout);
  idOrdenes = msgget(obtener_clave(CLAVE, CODIGO_ORDENES), 0666 | IPC_CREAT);
  idOk = msgget(obtener_clave(CLAVE, CODIGO_OK), 0666 | IPC_CREAT);
  idEmpleados =
      msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), IPC_CREAT | 0666);
  idListo =
      msgget(obtener_clave(CLAVE, CODIGO_LISTO), IPC_CREAT | 0666);

  msgctl(idEmpleados, IPC_RMID, 0);
  msgctl(idOk, IPC_RMID, 0);
  msgctl(idOrdenes, IPC_RMID, 0);
  msgctl(idListo, IPC_RMID, 0);


   idOrdenes = msgget(obtener_clave(CLAVE, CODIGO_ORDENES), 0666 | IPC_CREAT);
  idOk = msgget(obtener_clave(CLAVE, CODIGO_OK), 0666 | IPC_CREAT);
  idEmpleados =
      msgget(obtener_clave(CLAVE, CODIGO_EMPLEADOS), IPC_CREAT | 0666);
  idListo = msgget(obtener_clave(CLAVE, CODIGO_LISTO), IPC_CREAT | 0666);

  struct recibido init;
  init.type=max_gente;
  for (int i=0;i< MAXIMO_CLIENTES; i++) {
    msgsnd(idOk,&init ,longitud_recibido ,0 );
  }

  init.type=turno;
  msgsnd(idOk,&init ,longitud_recibido ,0 );
  init.type=turnoVIP;
  msgsnd(idOk,&init ,longitud_recibido ,0 );

  if (idEmpleados < 0 || idOk < 0 || idOrdenes < 0) {
    printf("Error creando colas de mensajes %d id Empleados, %d idOk, %d "
           "idOrdenes\n",
           idEmpleados, idOk, idOrdenes);
    exit(EXIT_FAILURE);
  }

  int pid = fork();

  if (pid == 0) {
    crear_laburantes();
     // sleep(5);
    exit(0);
  } else {
    if (pid > 0) {


      parte_clientes();
       wait(&pid);
    } else {
      perror("Error creando clientes\n");
      exit(EXIT_FAILURE);
    }
  }

msgctl(idEmpleados, IPC_RMID, 0);
  msgctl(idOk, IPC_RMID, 0);
  msgctl(idOrdenes, IPC_RMID, 0);
  msgctl(idListo, IPC_RMID, 0);
  return EXIT_SUCCESS;
}
