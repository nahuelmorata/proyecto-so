#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MSG_CTRL_SYNCHRONIZER 1
#define MSG_CTRL_GENERATOR_1  2
#define MSG_CTRL_GENERATOR_2  3
#define MSG_CTRL_WRITER_1     4
#define MSG_CTRL_WRITER_2     5

#define MSG_INFO_GENERATOR_1  6
#define MSG_INFO_GENERATOR_2  7
#define MSG_INFO_WRITER_1     8
#define MSG_INFO_WRITER_2     9

#define FINISH_ALL         0
#define SWITCH_TO_SALIDA_1 1
#define SWITCH_TO_SALIDA_2 2

#define KEY 1234

typedef struct {
	long type;
	int order;
} ctrl_message; 

typedef struct {
	long type;
	int number;
} info_message;

int longitud_ctrl_msg = sizeof(ctrl_message) - sizeof(long);
int longitud_info_msg = sizeof(info_message) - sizeof(long);

void controller();
void synchronizer();
void generator_1();
void generator_2();
void writer_1();
void writer_2();

int main(){
	
	srand(time(0));
	
	int pid = fork();
	if(pid != 0) {
		controller();
	} else {
		synchronizer();
	}
	
	return 0;
}

void controller(){
	
	int control, wstatus, queueid;
	
	ctrl_message msg;
	
	queueid = msgget(KEY, IPC_CREAT|0666);
	
	printf("Inicia la escritura en Salida1.txt \n");
	
	msg.type = MSG_CTRL_SYNCHRONIZER;
	msg.order = 1;
	msgsnd(queueid,&msg,longitud_ctrl_msg,0);
	
	while(1){
		printf("Ingresar valor: ");
		scanf("%d",&control);
		if(control>2 || control<0){
			printf("Valor invalido. \n");
			continue;
		}
		if(control == FINISH_ALL) printf("Inicio cierre. \n");
		msg.type = MSG_CTRL_SYNCHRONIZER;
		msg.order = control;
		msgsnd(queueid,&msg,longitud_ctrl_msg,0);
		if(control == FINISH_ALL) break;
		printf("Cambia la escritura a Salida%d.txt \n",control);
	}
	
	wait(&wstatus);
	printf("Controller: out \n");
}

void synchronizer(){
	
	int waitstatus, pid, i, queueid;
	
	ctrl_message ctrl_msg;
	info_message info_msg_1, info_msg_2;
	
	pid = fork();
	if(pid != 0 ) {
		pid = fork();
		if(pid != 0 ) {
			pid = fork();
			if(pid != 0 ) {
				pid = fork();
				if(pid == 0 ) {
					writer_2();
				} 
			} else {
				writer_1();
			}
		} else {
			generator_2();
		}
	} else {
		generator_1();
	}
	
	queueid = msgget(KEY, 0666);
	
	if(pid != 0) {
		
		while(1){
			// Lectura de control
			msgrcv(queueid,&ctrl_msg,longitud_ctrl_msg,MSG_CTRL_SYNCHRONIZER,IPC_NOWAIT);
			
			// Envio de control
			ctrl_msg.type = MSG_CTRL_GENERATOR_1;
			msgsnd(queueid,&ctrl_msg,longitud_ctrl_msg,0);
			ctrl_msg.type = MSG_CTRL_GENERATOR_2;
			msgsnd(queueid,&ctrl_msg,longitud_ctrl_msg,0);
			ctrl_msg.type = MSG_CTRL_WRITER_1;
			msgsnd(queueid,&ctrl_msg,longitud_ctrl_msg,0);
			ctrl_msg.type = MSG_CTRL_WRITER_2;
			msgsnd(queueid,&ctrl_msg,longitud_ctrl_msg,0);

			if(ctrl_msg.order == FINISH_ALL)break;
			
			// Lectura de numeros generados por 1 y 2
			msgrcv(queueid,&info_msg_1,longitud_info_msg,MSG_INFO_GENERATOR_1,0);
			msgrcv(queueid,&info_msg_2,longitud_info_msg,MSG_INFO_GENERATOR_2,0);
			
			if(ctrl_msg.order == SWITCH_TO_SALIDA_1){
				// Envio de numeros a 1
				info_msg_1.type = MSG_INFO_WRITER_1;
				info_msg_2.type = MSG_INFO_WRITER_1;
				msgsnd(queueid,&info_msg_1,longitud_info_msg,0);
				msgsnd(queueid,&info_msg_2,longitud_info_msg,0);
			}
			if(ctrl_msg.order == SWITCH_TO_SALIDA_2){
				// Envio de numeros a 2
				info_msg_1.type = MSG_INFO_WRITER_2;
				info_msg_2.type = MSG_INFO_WRITER_2;
				msgsnd(queueid,&info_msg_1,longitud_info_msg,0);
				msgsnd(queueid,&info_msg_2,longitud_info_msg,0);
			}
			
		}

		for(i=0;i<4;i++) wait(&waitstatus); //Espera a los 4 hijos
		
		printf("Synchronizer: out \n");
		
	}
}

void generator_1(){
	
	ctrl_message ctrl_msg;
	info_message info_msg;
	
	int queueid = msgget(KEY, 0666);
	
	while(1){
		msgrcv(queueid,&ctrl_msg,longitud_ctrl_msg,MSG_CTRL_GENERATOR_1,0);
		if(ctrl_msg.order == FINISH_ALL) break;
		info_msg.type = MSG_INFO_GENERATOR_1;
		info_msg.number = rand() % 10;
		msgsnd(queueid,&info_msg,longitud_info_msg,0);
	}
	
	printf("Generator 1: out \n");
	
}

void generator_2(){
	
	ctrl_message ctrl_msg;
	info_message info_msg;
	
	int queueid = msgget(KEY, 0666);
	
	while(1){
		msgrcv(queueid,&ctrl_msg,longitud_ctrl_msg,MSG_CTRL_GENERATOR_2,0);
		if(ctrl_msg.order == FINISH_ALL) break;
		info_msg.type = MSG_INFO_GENERATOR_2;
		info_msg.number = rand() % 10;
		msgsnd(queueid,&info_msg,longitud_info_msg,0);
	}
	
	printf("Generator 2: out \n");
	
}

void writer_1(){
	
	int result;
		
	ctrl_message ctrl_msg;
	info_message info_msg_1, info_msg_2;
	
	int queueid = msgget(KEY, 0666);
		
	FILE * salida1 = fopen("Salida1.txt","w");
		
	while(1){
		msgrcv(queueid,&ctrl_msg,longitud_ctrl_msg,MSG_CTRL_WRITER_1,0);
		if(ctrl_msg.order == FINISH_ALL) break;
		result = msgrcv(queueid,&info_msg_1,longitud_info_msg,MSG_INFO_WRITER_1,IPC_NOWAIT);
		result = result + msgrcv(queueid,&info_msg_2,longitud_info_msg,MSG_INFO_WRITER_1,IPC_NOWAIT);
		if(result > 0) fprintf(salida1,"Gen1: %d - Gen2:%d \n",info_msg_1.number,info_msg_2.number);
	}
		
	fclose(salida1);
	printf("Writer 1: out \n");
		
}

void writer_2(){
	
	int result;
	
	ctrl_message ctrl_msg;
	info_message info_msg_1, info_msg_2;
	
	int queueid = msgget(KEY, 0666);
		
	FILE * salida2 = fopen("Salida2.txt","w");
	
	while(1){
		msgrcv(queueid,&ctrl_msg,longitud_ctrl_msg,MSG_CTRL_WRITER_2,0);
		if(ctrl_msg.order == FINISH_ALL) break;
		result = msgrcv(queueid,&info_msg_1,longitud_info_msg,MSG_INFO_WRITER_2,IPC_NOWAIT);
		result = result + msgrcv(queueid,&info_msg_2,longitud_info_msg,MSG_INFO_WRITER_2,IPC_NOWAIT);
		if(result > 0) fprintf(salida2,"Gen1: %d - Gen2:%d \n",info_msg_1.number,info_msg_2.number);
	}
	
	fclose(salida2);
	printf("Writer 2: out \n");

}

