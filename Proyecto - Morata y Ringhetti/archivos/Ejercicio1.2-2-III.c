#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
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

#define KEY 987654

#define LLENO_PRIMERA  1
#define LLENO_BUSINESS 2
#define LLENO_TURISTA  3
#define LIBRE_PRIMERA  4
#define LIBRE_BUSINESS 5
#define LIBRE_TURISTA  6
#define TURNO_PRIMERA  8
#define TURNO_BUSINESS 9
#define TURNO_TURISTA  10
#define ZARPO          11

typedef struct {
	long type;
} msg;

#define SIZE sizeof(msg) - sizeof(long)

int p_qid;

void pasajeroPrimera(int id);
void pasajeroBusiness(int id);
void pasajeroTurista(int id);
void ticketeria();

int main(){
	
	int i, pid;
	msg mensaje;
	
	p_qid = msgget(KEY,IPC_CREAT|0666);
	if(p_qid == -1) printf("Queue error ticketeria \n");
	
	mensaje.type = LIBRE_PRIMERA;
	for(i=0;i<MAX_PRIMERA;i++) msgsnd(p_qid,&mensaje,SIZE,0);
	mensaje.type = LIBRE_BUSINESS;
	for(i=0;i<MAX_BUSINESS;i++) msgsnd(p_qid,&mensaje,SIZE,0);
	mensaje.type = LIBRE_TURISTA;
	for(i=0;i<MAX_TURISTA;i++) msgsnd(p_qid,&mensaje,SIZE,0); 
	
	for(i=0;i<CANT_PRIMERA;i++){
		pid = fork();
		if(pid == 0){
			pasajeroPrimera(i);
			break;
		}
	}
	for(i=0;i<CANT_BUSINESS;i++){
		pid = fork();
		if(pid == 0){
			pasajeroBusiness(i);
			break;
		}
	}
	for(i=0;i<CANT_TURISTA;i++){
		pid = fork();
		if(pid == 0){
			pasajeroTurista(i);
			break;
		}
	}
	
	if(pid != 0) ticketeria();
	
	return 0;
}

void pasajeroPrimera(int id){
	
	int qid = msgget(KEY,0666);
	msg mensaje;
	
	while(1){
		msgrcv(qid,&mensaje,SIZE,TURNO_PRIMERA,0);
	    printf("Soy el pasajero de PRIMERA N°%d \n",id);	
		printf("Quiero un boleto en PRIMERA. \n");
		sleep(1);
		
		if(msgrcv(p_qid,&mensaje,SIZE,LIBRE_PRIMERA,IPC_NOWAIT) < 0){ 
			mensaje.type = LLENO_PRIMERA;
			msgsnd(qid,&mensaje,SIZE,0);
		} else {
			printf("Espero en la fila de PRIMERA. \n");
			sleep(1);
			mensaje.type = TURNO_PRIMERA;
			msgsnd(qid,&mensaje,SIZE,0);
			msgrcv(qid,&mensaje,SIZE,ZARPO,0);
		}
	}	
	
	exit(0);
	
}

void pasajeroBusiness(int id){
	
	int qid = msgget(KEY,0666);
	msg mensaje;
	
	while(1){
		msgrcv(qid,&mensaje,SIZE,TURNO_BUSINESS,0);
	    printf("Soy el pasajero de BUSINESS N°%d \n",id);	
		printf("Quiero un boleto en BUSINESS. \n");
		sleep(1);
		
		if(msgrcv(p_qid,&mensaje,SIZE,LIBRE_BUSINESS,IPC_NOWAIT) < 0){ 
			mensaje.type = LLENO_BUSINESS;
			msgsnd(qid,&mensaje,SIZE,0);
		} else {
			printf("Espero en la fila de BUSINESS. \n");
			sleep(1);
			mensaje.type = TURNO_BUSINESS;
			msgsnd(qid,&mensaje,SIZE,0);
			msgrcv(qid,&mensaje,SIZE,ZARPO,0);
		}
	}	
	
	exit(0);
	
}

void pasajeroTurista(int id){
	
	int qid = msgget(KEY,0666);
	msg mensaje;
	
	while(1){
		msgrcv(qid,&mensaje,SIZE,TURNO_TURISTA,0);
	    printf("Soy el pasajero de TURISTA N°%d \n",id);	
		printf("Quiero un boleto en TURISTA. \n");
		sleep(1);
		
		if(msgrcv(p_qid,&mensaje,SIZE,LIBRE_TURISTA,IPC_NOWAIT) < 0){ 
			mensaje.type = LLENO_TURISTA;
			msgsnd(qid,&mensaje,SIZE,0);
		} else {
			printf("Espero en la fila de TURISTA. \n");
			sleep(1);
			mensaje.type = TURNO_TURISTA;
			msgsnd(qid,&mensaje,SIZE,0);
			msgrcv(qid,&mensaje,SIZE,ZARPO,0);
		}
	}	
	
	exit(0);
	
}

void ticketeria(){
	
	int i;
	msg mensaje;	
	
	printf("Se abre la ticketería. \n");
	printf("-------------------------------------------------------------\n");
	mensaje.type = TURNO_PRIMERA;
	msgsnd(p_qid,&mensaje,SIZE,0);
	mensaje.type = TURNO_BUSINESS;
	msgsnd(p_qid,&mensaje,SIZE,0);
	mensaje.type = TURNO_TURISTA;
	msgsnd(p_qid,&mensaje,SIZE,0);
	
	while(1){
		msgrcv(p_qid,&mensaje,SIZE,LLENO_PRIMERA,0);
		msgrcv(p_qid,&mensaje,SIZE,LLENO_BUSINESS,0);
		msgrcv(p_qid,&mensaje,SIZE,LLENO_TURISTA,0);
		printf("-------------------------------------------------------------\n");
		printf("El barco esta lleno! Vamos a zarpar. \n");
		sleep(1);
		mensaje.type = ZARPO;
		for(i=0;i<MAX_PASAJEROS;i++)msgsnd(p_qid,&mensaje,SIZE,0);
		printf("El barco esta llego a destino, se bajan los pasajeros. \n");
		sleep(1);
		
		mensaje.type = LIBRE_PRIMERA;
		for(i=0;i<MAX_PRIMERA;i++)msgsnd(p_qid,&mensaje,SIZE,0);
		
		mensaje.type = LIBRE_BUSINESS;
		for(i=0;i<MAX_BUSINESS;i++)msgsnd(p_qid,&mensaje,SIZE,0);
		
		mensaje.type = LIBRE_TURISTA;
		for(i=0;i<MAX_TURISTA;i++)msgsnd(p_qid,&mensaje,SIZE,0);
		
		printf("Ya se bajaron los pasajeros, volvemos al puerto. \n");
		sleep(1);
		printf("El barco volvio al puerto! Reabrimos la ticketeria. \n");
		sleep(1);
		printf("Que pase el siguiente!\n");
		printf("-------------------------------------------------------------\n");
		sleep(1);
		
		mensaje.type = TURNO_PRIMERA;
		msgsnd(p_qid,&mensaje,SIZE,0);
		mensaje.type = TURNO_BUSINESS;
		msgsnd(p_qid,&mensaje,SIZE,0);
		mensaje.type = TURNO_TURISTA;
		msgsnd(p_qid,&mensaje,SIZE,0);
	}
	
	exit(0);
	
}




