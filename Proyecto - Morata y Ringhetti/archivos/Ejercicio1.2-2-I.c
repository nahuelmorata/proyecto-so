#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_PRIMERA  20
#define MAX_BUSINESS 30
#define MAX_TURISTA  50

#define MAX_PASAJEROS MAX_PRIMERA+MAX_BUSINESS+MAX_TURISTA

#define CANT_PRIMERA  35
#define CANT_BUSINESS 45
#define CANT_TURISTA  75

void * pasajeroPrimera(void * data);
void * pasajeroBusiness(void * data);
void * pasajeroTurista(void * data);
void * ticketeria();

sem_t semPrimeraLibre, semPrimeraLleno, semBusinessLibre, semBusinessLleno, semTuristaLibre, semTuristaLleno, semTurnoPrimera, semTurnoBusiness, semTurnoTurista, semBarcoZarpo;

pthread_t pasajerosPrimera[CANT_PRIMERA];
pthread_t pasajerosBusiness[CANT_BUSINESS];
pthread_t pasajerosTurista[CANT_TURISTA];
pthread_t hiloTicketeria;

int main(){

	int i;

	sem_init(&semPrimeraLibre,0,MAX_PRIMERA); 
	sem_init(&semPrimeraLleno,0,0);   
	sem_init(&semBusinessLibre,0,MAX_BUSINESS); 
	sem_init(&semBusinessLleno,0,0); 
	sem_init(&semTuristaLibre,0,MAX_TURISTA);   
	sem_init(&semTuristaLleno,0,0); 				
	sem_init(&semTurnoPrimera,0,0);					
	sem_init(&semTurnoBusiness,0,0);					
	sem_init(&semTurnoTurista,0,0);					
	sem_init(&semBarcoZarpo,0,0);				
	
	for(i=0;i<CANT_PRIMERA;i++){
		pthread_create(&pasajerosPrimera[i],NULL,&pasajeroPrimera,(void *) i);
	}
	for(i=0;i<CANT_BUSINESS;i++){
		pthread_create(&pasajerosBusiness[i],NULL,&pasajeroBusiness,(void *) i);
	}
	for(i=0;i<CANT_TURISTA;i++){
		pthread_create(&pasajerosTurista[i],NULL,&pasajeroTurista,(void *) i);
	}
	pthread_create(&hiloTicketeria,NULL,&ticketeria,NULL);
	
	for(i=0;i<CANT_PRIMERA;i++){
		pthread_join(pasajerosPrimera[i],NULL);
	}
	for(i=0;i<CANT_BUSINESS;i++){
		pthread_join(pasajerosBusiness[i],NULL);
	}
	for(i=0;i<CANT_TURISTA;i++){
		pthread_join(pasajerosTurista[i],NULL);
	}
	pthread_join(hiloTicketeria,NULL);

	printf("Se finni \n");

	return 0;
	
}

void * ticketeria(){
	
	int i;
	
	printf("Se abre la ticketería. \n");
	printf("-------------------------------------------------------------\n");
	sem_post(&semTurnoPrimera);
	sem_post(&semTurnoBusiness);
	sem_post(&semTurnoTurista);
	
	while(1){
		sem_wait(&semPrimeraLleno);
		sem_wait(&semBusinessLleno);
		sem_wait(&semTuristaLleno);
		printf("-------------------------------------------------------------\n");
		printf("El barco esta lleno! Vamos a zarpar. \n");
		sleep(1);
		for(i=0;i<MAX_PASAJEROS;i++)sem_post(&semBarcoZarpo);
		printf("El barco esta llego a destino, se bajan los pasajeros. \n");
		sleep(1);
		
		for(i=0;i<MAX_PRIMERA;i++)sem_post(&semPrimeraLibre);
		
		for(i=0;i<MAX_BUSINESS;i++)sem_post(&semBusinessLibre);
		
		for(i=0;i<MAX_TURISTA;i++)sem_post(&semTuristaLibre);
		
		printf("Ya se bajaron los pasajeros, volvemos al puerto. \n");
		sleep(1);
		printf("El barco volvio al puerto! Reabrimos la ticketeria. \n");
		sleep(1);
		printf("Que pase el siguiente!\n");
		printf("-------------------------------------------------------------\n");
		sleep(1);
		sem_post(&semTurnoPrimera);
		sem_post(&semTurnoTurista);
		sem_post(&semTurnoBusiness);
	}
	
}

void * pasajeroPrimera(void * data){

	int id = (int) data;
	
	while(1){
		sem_wait(&semTurnoPrimera);
	    printf("Soy el pasajero de PRIMERA N°%d \n",id);	
		printf("Quiero un boleto en PRIMERA. \n");
		sleep(1);
		
		if(sem_trywait(&semPrimeraLibre) != 0){ 
			sem_post(&semPrimeraLleno);
		} else {
			printf("Espero en la fila de PRIMERA. \n");
			sleep(1);
			sem_post(&semTurnoPrimera);
			sem_wait(&semBarcoZarpo);
		}
	}	

}


void * pasajeroBusiness(void * data){

	int id = (int) data;
	
	while(1){
		sem_wait(&semTurnoBusiness);
		printf("Soy el pasajero de BUSINESS N°%d \n",id);
		printf("Quiero un boleto en BUSINESS. \n");
		sleep(1);

		if(sem_trywait(&semBusinessLibre) != 0){ 
			sem_post(&semBusinessLleno);
		} else {
			printf("Espero en la fila de BUSINESS. \n");
			sleep(1);
			sem_post(&semTurnoBusiness);
			sem_wait(&semBarcoZarpo);		
		}
	}

}

void * pasajeroTurista(void * data){

	int id = (int) data;
	
	while(1){
		sem_wait(&semTurnoTurista);
		printf("Soy el pasajero DE TURISTA N°%d \n",id);	
		printf("Quiero un boleto en TURISTA. \n");
		sleep(1);

		if(sem_trywait(&semTuristaLibre) != 0){ 
			sem_post(&semTuristaLleno);
		} else {
			printf("Espero en la fila de TURISTA. \n");
			sleep(1);
			sem_post(&semTurnoTurista);
			sem_wait(&semBarcoZarpo);
		}
	}

}
