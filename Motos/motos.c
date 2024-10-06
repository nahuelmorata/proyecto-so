#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t sem_rueda;
sem_t sem_chasis;
sem_t sem_motor;
sem_t sem_pintura;
sem_t sem_extra;
sem_t sem_par;
pthread_mutex_t pintura_mutex=PTHREAD_MUTEX_INITIALIZER;

int contador_motos_ensambladas;
void* operario_rueda(void *arg){
free(arg);
  while(1){
        sem_wait(&sem_rueda);
        printf("Armando rueda..\n");
        sleep(1);
        printf("Mandando señal a chasis..\n");
        sem_post(&sem_chasis);
    }
return NULL;
}


void* segundo_operario_chasis(){
while(1){
    sem_wait(&sem_chasis);
    sem_wait(&sem_chasis);
        printf("Armando el chasis...\n");
        sleep(1);
        printf("Mandando señal a motor...\n");
    sem_post(&sem_motor);
}
return NULL;
}


void* tercer_operario_motor(void* arg){
    free(arg);
while(1){
sem_wait(&sem_motor);
printf("Armando el motor...\n");
sleep(1);
printf("Mandando señal a pintura...\n");
sem_post(&sem_pintura);
}
return NULL;
}
void* pintor(void *color){

char* colorP = *(char **) color;
while(1){
  sem_wait(&sem_pintura);
  printf("Pintando la moto de: %s..\n", colorP);
  sleep(1);
  printf("Moto pintada de: %s..\n",colorP);

  pthread_mutex_lock(&pintura_mutex);
    if(sem_trywait(&sem_par)==0){
      sem_post(&sem_extra);

    }else {
        printf("Moto terminada\n");
        sem_post(&sem_par);
        sem_post(&sem_rueda);
        sem_post(&sem_rueda);
    }
    pthread_mutex_unlock(&pintura_mutex);
}
return NULL;
}
void* operario_equipacion_extra(void *arg){
    free(arg);
while(1){
  sem_wait(&sem_extra);
    printf(" agregando equipación extra..\n");
    sleep(1);
    printf(" equipación extra agregada. \n");
    printf("Moto terminada. \n");
  sem_post(&sem_rueda);
  sem_post(&sem_rueda);

}
return NULL;
}

int main(){
    pthread_t pintorR,pintorV,tercer_operario_motorT,operario_equipacion_extraT,operario_ruedaT,segundo_operario_chasisT;
    sem_init(&sem_rueda,0 ,2 );
    sem_init(&sem_chasis,0 ,0 );
    sem_init(&sem_motor,0 ,0 );
    sem_init(&sem_pintura,0 ,0 );
    sem_init(&sem_par,0 ,0 );
    sem_init(&sem_extra,0 ,0 );

    char * verde="verde";
    char * rojo="rojo";
    pthread_create(&pintorV,NULL , pintor,&verde );
    pthread_create(&pintorR,NULL , pintor,&rojo );
    pthread_create(&operario_ruedaT,NULL , operario_rueda,NULL );
    pthread_create(&tercer_operario_motorT,NULL , tercer_operario_motor,NULL );
    pthread_create(&segundo_operario_chasisT,NULL , segundo_operario_chasis,NULL );
    pthread_create(&operario_equipacion_extraT,NULL , operario_equipacion_extra,NULL );

    pthread_join(pintorR,NULL );
    pthread_join(tercer_operario_motorT, NULL);
    pthread_join(operario_equipacion_extraT,NULL );
    pthread_join(operario_ruedaT,NULL );
    pthread_join(pintorV,NULL );
    pthread_join(segundo_operario_chasisT,NULL );



    sem_destroy(&sem_rueda);
    sem_destroy(&sem_chasis);
    sem_destroy(&sem_motor);
    sem_destroy(&sem_pintura);
    sem_destroy(&sem_par);
    sem_destroy(&sem_extra);

    return 0;
}
