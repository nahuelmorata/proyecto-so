#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SIZE sizeof(int)
#define TRASH -1

void controller();
void synchronizer();
void generator_1();
void generator_2();
void writer_1();
void writer_2();

int pipe_generator1_synchronizer[2];
int pipe_generator2_synchronizer[2];
int pipe_synchronizer_generator1[2];
int pipe_synchronizer_generator2[2];
int pipe_synchronizer_writer1[2];
int pipe_synchronizer_writer2[2];
int pipe_controller_synchronizer[2];

int main() {
	
	if(pipe(pipe_controller_synchronizer)<0) exit(1);
	if(fcntl(pipe_controller_synchronizer[0],F_SETFL,O_NONBLOCK)<0) exit(2); //Que no sea bloqueante para que los ciclos no esten ligados a los ingresos
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
	
	int control, wstatus;
	
	close(pipe_controller_synchronizer[0]); //Cierro lectura -> Controller escribe a Synchronizer
	
	control = 1; //Inicia en salida 1
	write(pipe_controller_synchronizer[1],&control,SIZE);
	
	printf("Inicia la escritura en Salida1.txt \n");
	
	while(1){
		printf("Ingresar valor: ");
		scanf("%d",&control);
		if(control>2 || control<0){
			printf("Valor invalido. \n");
			continue;
		}
		if(control == 0) printf("Inicio cierre. \n");
		write(pipe_controller_synchronizer[1],&control,SIZE);
		if(control == 0) break;
		printf("Cambia la escritura a Salida%d.txt \n",control);
	}
	
	wait(&wstatus);
	close(pipe_controller_synchronizer[1]);
	printf("Controller: out \n");
}

void synchronizer(){
	
	int control, num1, num2, waitstatus, pid, i;
	
	int trash = TRASH;
	
	close(pipe_controller_synchronizer[1]); //Cierro escritura -> Synchronizer lee de Controller
	
	if(pipe(pipe_generator1_synchronizer)<0) exit(1);
	if(pipe(pipe_generator2_synchronizer)<0) exit(1);
	if(pipe(pipe_synchronizer_generator1)<0) exit(1);
	if(pipe(pipe_synchronizer_generator2)<0) exit(1);
	if(pipe(pipe_synchronizer_writer1)<0) exit(1);
	if(pipe(pipe_synchronizer_writer2)<0) exit(1);
	
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
	
	if(pid != 0) {
		
		close(pipe_generator1_synchronizer[1]); //Cierro escritura -> Synchronizer lee de Generator1
		close(pipe_generator2_synchronizer[1]); //Cierro escritura -> Synchronizer lee de Generator2
		close(pipe_synchronizer_generator1[0]); //Cierro lectura -> Synchronizer escribe a Generator1
		close(pipe_synchronizer_generator2[0]); //Cierro lectura -> Synchronizer escribe a Generator2
		close(pipe_synchronizer_writer1[0]); //Cierro lectura -> Synchronizer escribe a Writer1
		close(pipe_synchronizer_writer2[0]); //Cierro lectura -> Synchronizer escribe a Writer2
		
		while(1){
			read(pipe_controller_synchronizer[0],&control,SIZE);
			write(pipe_synchronizer_generator1[1],&control,SIZE);
			write(pipe_synchronizer_generator2[1],&control,SIZE);
			if(control == 0) {
				//Mando basura para indicar la finalizacion a los writers
				write(pipe_synchronizer_writer1[1],&trash,SIZE);
				write(pipe_synchronizer_writer1[1],&trash,SIZE);
				write(pipe_synchronizer_writer2[1],&trash,SIZE);
				write(pipe_synchronizer_writer2[1],&trash,SIZE);
				break;
			}
			read(pipe_generator1_synchronizer[0],&num1,SIZE);
			read(pipe_generator2_synchronizer[0],&num2,SIZE);
			if(control == 1){
				write(pipe_synchronizer_writer1[1],&num1,SIZE);
				write(pipe_synchronizer_writer1[1],&num2,SIZE);	
			}
			if(control == 2){
				write(pipe_synchronizer_writer2[1],&num1,SIZE);
				write(pipe_synchronizer_writer2[1],&num2,SIZE);	
			}
		}

		for(i=0;i<4;i++) wait(&waitstatus); //Espera a los 4 hijos
		close(pipe_generator1_synchronizer[0]); 
		close(pipe_generator2_synchronizer[0]); 
		close(pipe_synchronizer_generator1[1]); 
		close(pipe_synchronizer_generator2[1]); 
		close(pipe_synchronizer_writer1[1]); 
		close(pipe_synchronizer_writer2[1]); 
		printf("Synchronizer: out \n");
		
	}
}

void generator_1(){
	
	int control, num;
	
	close(pipe_generator1_synchronizer[0]); //Cierro lectura -> Generator1 escribe a Synchronizer
	close(pipe_synchronizer_generator1[1]); //Cierro escritura -> Generator1 lee de Synchronizer
	
	while(1){
		sleep(0.5);
		read(pipe_synchronizer_generator1[0],&control,SIZE);
		if(control == 0) break;
		num = (rand() % 10);
		write(pipe_generator1_synchronizer[1],&num,SIZE);
	}
	
	close(pipe_generator1_synchronizer[1]);
	close(pipe_synchronizer_generator1[0]); 
	printf("Generator 1: out \n");
	
}

void generator_2(){
	
	int control, num;
	
	close(pipe_generator2_synchronizer[0]); //Cierro lectura -> Generator1 escribe a Synchronizer
	close(pipe_synchronizer_generator2[1]); //Cierro escritura -> Generator2 lee de Synchronizer
	
	while(1){
		sleep(0.5);
		read(pipe_synchronizer_generator2[0],&control,SIZE);
		if(control == 0) break;
		num = (rand() % 10);
		write(pipe_generator2_synchronizer[1],&num,SIZE);
	}
	
	close(pipe_generator2_synchronizer[1]); 
	close(pipe_synchronizer_generator2[0]);
	printf("Generator 2: out \n");
	
}

void writer_1(){
		
		int num1, num2;
		
		int trash = TRASH;
		
		FILE * salida1 = fopen("Salida1.txt","w");
		
		close(pipe_synchronizer_writer1[1]); //Cierro escritura -> Writer1 lee de Synchronizer
		
		while(1){
			sleep(0.5);
			read(pipe_synchronizer_writer1[0],&num1,SIZE);
			read(pipe_synchronizer_writer1[0],&num2,SIZE);
			if(num1 == trash) break;
			fprintf(salida1,"Gen1: %d - Gen2:%d \n",num1,num2);
		}
		
		fclose(salida1);
		close(pipe_synchronizer_writer1[0]);
		printf("Writer 1: out \n");
		
}

void writer_2(){
	
	int num1, num2;
	
	int trash = TRASH;
	
	FILE * salida2 = fopen("Salida2.txt","w");
	
	close(pipe_synchronizer_writer2[1]); //Cierro escritura -> Writer2 lee de Synchronizer
	
	while(1){
		sleep(0.5);
		read(pipe_synchronizer_writer2[0],&num1,SIZE);
		read(pipe_synchronizer_writer2[0],&num2,SIZE);
		if(num1 == trash) break;
		fprintf(salida2,"Gen1: %d - Gen2:%d \n",num1,num2);
	}
	
	fclose(salida2);
	close(pipe_synchronizer_writer2[0]);
	printf("Writer 2: out \n");
	
}
