#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#define NUMERO_RENOS 9
#define NUMERO_ELFOS 3



int santa_seguir;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
sem_t cant_renos;
sem_t cant_elfos;
sem_t reno_espera,reno_resuelto;
sem_t elfo_espera,elfo_resuelto;
sem_t santa_resuelto;
sem_t santa_atiende_elfo,santa_atiende_reno;
sem_t elfos_problema,renos_trineo;

void *santa(void *args){

  args=NULL;
  free(args);
  while(santa_seguir) {
  
  for (int i=0;i<NUMERO_RENOS;i++) {
    sem_wait(&reno_espera);
  }
    sleep(1);

  for (int i=0;i<NUMERO_ELFOS;i++) {
    sem_wait(&elfo_espera);
  }
  sleep(1);

  for (int i=0;i<NUMERO_RENOS;i++) {
   sem_post(&santa_atiende_reno);
    printf("Atando reno\n");
    sem_wait(&reno_resuelto);
   sem_post(&santa_resuelto); // Segundo rendezvous para no atender a un reno sin terminar a otro
    sleep(1);

  }
    sleep(1);
    printf("Todos los renos atados\n");
    sem_post(&cant_renos);

    for (int i=0;i<NUMERO_ELFOS;i++) {
   sem_post(&santa_atiende_elfo);
    printf("Elfo siendo atendido\n");
   sem_wait(&elfo_resuelto);
   sem_post(&santa_resuelto);
    sleep(1);
  }
    sleep(1);
    printf("Todos los elfos atendidos\n");
    sem_post(&cant_elfos);
   printf("======Renos atados y elfos atendidos=====\n");
    sleep(5);
  }

  pthread_exit(NULL);
}

void *renos(void *args)
{  args=NULL;
  free(args);

  sem_wait(&renos_trineo);
  pthread_mutex_lock(&mutex);
      sem_post(&cant_renos);
  pthread_mutex_unlock(&mutex);

  sem_post(&reno_espera);
  sem_wait(&santa_atiende_reno); //primer rendezvous para que santa atienda un reno
    printf("Santa atando\n");
  sem_post(&reno_resuelto);
  sem_wait(&santa_resuelto);

  pthread_mutex_lock(&mutex);

    if(sem_trywait(&cant_renos)){
      for (int i=0;i<NUMERO_RENOS;i++) {
        sem_post(&renos_trineo);
      }
    }
  pthread_mutex_unlock(&mutex);
    sleep(1);
  printf("Fui atado\n");
  pthread_exit(NULL);
}

void *elfos(void *args)
{
  args=NULL;
  free(args);
  sem_wait(&elfos_problema);
  pthread_mutex_lock(&mutex);
    sem_post(&cant_elfos);
  pthread_mutex_unlock(&mutex);
  sem_post(&elfo_espera);
  sem_wait(&santa_atiende_elfo);
    printf("Santa resolviendo problema\n");
  sem_post(&elfo_resuelto);
  sem_wait(&santa_resuelto);
  pthread_mutex_lock(&mutex);

    if(sem_trywait(&cant_elfos)){
      for (int i=0;i<NUMERO_ELFOS;i++) {
         sem_post(&elfos_problema);
      }
    }

  pthread_mutex_unlock(&mutex);

    sleep(1);
  printf("Problema resuelto\n");
  pthread_exit(NULL);
}


int main() {

   
  //El numero de elfos debe ser multiplo de NUMERO_ELFOS 
  // El numero de renos debe ser multiplo de NUMERO_RENOS, similar a los elfos
  int total_renos=NUMERO_RENOS;
  int total_elfos=NUMERO_ELFOS;
  pthread_t santaT;
  pthread_t renosT[total_renos];
  pthread_t elfosT[total_elfos];

  santa_seguir=1;
  sem_init(&cant_elfos,0,0);
  sem_init(&cant_renos,0,0);
  sem_init(&reno_espera,0,0);
  sem_init(&elfo_espera,0,0);
  sem_init(&reno_resuelto,0,0);
  sem_init(&elfo_resuelto,0,0);
  sem_init(&santa_resuelto,0,0);
  sem_init(&renos_trineo,0,NUMERO_RENOS);
  sem_init(&elfos_problema,0,NUMERO_ELFOS);
  sem_init(&santa_atiende_reno,0,0);
  sem_init(&santa_atiende_elfo,0,0);

  if((total_renos%NUMERO_RENOS)!=0){
    perror("Error, programa imposible de completar, numero de renos erroneo");
    exit(EXIT_FAILURE);
  }
   if((total_elfos%NUMERO_ELFOS)!=0){
    perror("Error, programa imposible de completar, numero de elfos erroneo");
    exit(EXIT_FAILURE);
  } 
  pthread_create(&santaT,NULL,&santa,NULL);
  for (int i=0;i<total_renos;i++) {
    pthread_create(&renosT[i],NULL,&renos,NULL);
  }

  for (int i=0;i<total_elfos;i++) {
    pthread_create(&elfosT[i],NULL,&elfos,NULL);
  }

  for (int i=0;i<total_renos;i++) {
    pthread_join(renosT[i],NULL);
  }

  for (int i=0;i<total_elfos;i++) {
    pthread_join(elfosT[i],NULL);
  }
  santa_seguir=0;
  pthread_join(santaT,NULL);
  return EXIT_SUCCESS;
}

