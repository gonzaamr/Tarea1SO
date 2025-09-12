#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

void buffer(int tipoMensaje, char tiempo[20], pid_t pid, char msj[100], int fd){
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "%d|%s|%d|%s\n", tipoMensaje, tiempo, pid, msj);
	write(fd, buffer, strlen(buffer));
}

void tiempo(char *tiempo_str){
	time_t t = time(NULL);
	struct tm tm_info;
	localtime_r(&t, &tm_info);
	strftime(tiempo_str, 20, "%H:%M:%S", &tm_info);
}

void registro();
void leer(int fo){
	char rcv_buffer[200];
	ssize_t n = read(fo, rcv_buffer, sizeof(rcv_buffer)-1);
	if(n > 0){
		rcv_buffer[n] = '\0';
		printf("%s", rcv_buffer);
	} else{
		usleep(100000);
	}
}

void escribir(int fd, int fo, pid_t pid){
    	    char mensaje[100];
	    char tiempo_str[20];
	    tiempo(tiempo_str);
            fgets(mensaje, sizeof(mensaje), stdin);
            mensaje[strcspn(mensaje, "\n")] = 0;

            if(strcmp(mensaje, "/report") == 0){
                printf("Ingrese el PID de usuario que desea reportar:");
                fgets(mensaje, sizeof(mensaje), stdin);
                mensaje[strcspn(mensaje, "\n")] = 0;
		buffer(2, tiempo_str, pid, mensaje, fd);

            } else if(strcmp(mensaje, "/exit") == 0){
                strcpy(mensaje, "Desconexión");
		buffer(3, tiempo_str, pid, mensaje, fd);
                usleep(100000);
                close(fd);
                close(fo);
                exit(0);

            } else if(strcmp(mensaje, "/clonar") == 0){
        		    pid_t clone = fork();
        		    if (clone == 0){
        	    	        registro();
                        }
            } else{
        		buffer(1, tiempo_str, pid, mensaje, fd);
            }
}

void registro(){
    pid_t pid = getpid();  // PID correcto del proceso que está ejecutando registro

    int fd = open("/tmp/chat_in", O_WRONLY);
    if(fd == -1){
        perror("No se ha iniciado servidor");
        return;
    }

    buffer(0, 0, pid,  0, fd);  // registro con PID real del proceso

    pid_t child = fork(); // fork para lector/escritor

    char fifo_out[50];
    snprintf(fifo_out, sizeof(fifo_out), "/tmp/chat_out_%d", pid);
    int fo;
    while ((fo = open(fifo_out, O_RDONLY)) == -1) {
        usleep(100000);
    }

    if(child == 0){
	close(fd);
        while(1){
            leer(fo);
        }
    } else {
	close(fo);
        while(1){
            escribir(fd, fo, pid); // pasa PID real del proceso
        }
    }
}


int main(){
    registro();
    return 0;
}
