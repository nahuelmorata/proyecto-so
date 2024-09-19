#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void * A();
void * B();
void * C();
void * D();
void * E();
void * F();

sem_t semA, semB, semC, semD, semE, semF, semAux; 

pthread_t hiloA, hiloB, hiloC, hiloD, hiloE, hiloF;

// ABCEF ABDEF

int main() {
	sem_init(&semA,0,1);
	sem_init(&semB,0,0);
	sem_init(&semC,0,1);
	sem_init(&semD,0,0);
	sem_init(&semE,0,0);
	sem_init(&semF,0,0);
	sem_init(&semAux,0,0);
	
	pthread_create(&hiloA,NULL,A,NULL);
	pthread_create(&hiloB,NULL,B,NULL);
	pthread_create(&hiloC,NULL,C,NULL);
	pthread_create(&hiloD,NULL,D,NULL);
	pthread_create(&hiloE,NULL,E,NULL);
	pthread_create(&hiloF,NULL,F,NULL);
	
	pthread_join(hiloA,NULL);
	pthread_join(hiloB,NULL);
	pthread_join(hiloC,NULL);
	pthread_join(hiloD,NULL);
	pthread_join(hiloE,NULL);
	pthread_join(hiloF,NULL);
	
	return 0;
}

void * A(){
	while(1){
		sem_wait(&semA);
		printf("-------------------------------\n");
		printf("Comenzando la producción de A: \n");
		for(int i=1;i<=2;i++){
			sleep(1);
			printf("%d) Produciendo A. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de A. \n");
		sem_post(&semB);
	}
}

void * B(){
	while(1){
		sem_wait(&semB);
		printf("-------------------------------\n");
		printf("Comenzando la producción de B: \n");
		for(int i=1;i<=1;i++){
			sleep(1);
			printf("%d) Produciendo B. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de B. \n");
		sem_post(&semAux);
	}
}

void * C(){
	while(1){
		sem_wait(&semC);
		sem_wait(&semAux);
		printf("-------------------------------\n");
		printf("Comenzando la producción de C: \n");
		for(int i=1;i<=3;i++){
			sleep(1);
			printf("%d) Produciendo C. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de C. \n");
		sem_post(&semD);
		sem_post(&semE);
	}
}

void * D(){
	while(1){
		sem_wait(&semD);
		sem_wait(&semAux);
		printf("-------------------------------\n");
		printf("Comenzando la producción de D: \n");
		for(int i=1;i<=7;i++){
			sleep(1);
			printf("%d) Produciendo D. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de D. \n");
		sem_post(&semC);
		sem_post(&semE);
	}
}

void * E(){
	while(1){
		sem_wait(&semE);
		printf("-------------------------------\n");
		printf("Comenzando la producción de E: \n");
		for(int i=1;i<=2;i++){
			sleep(1);
			printf("%d) Produciendo E. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de E. \n");
		sem_post(&semF);
	}
}

void * F(){
	while(1){
		sem_wait(&semF);
		printf("-------------------------------\n");
		printf("Comenzando la producción de F: \n");
		for(int i=1;i<=3;i++){
			sleep(1);
			printf("%d) Produciendo F. \n",i);
		}
		sleep(1);
		printf("Terminando la producción de F. \n");
		sem_post(&semA);
	}
}
