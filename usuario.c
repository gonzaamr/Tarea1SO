#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


void tiempo(char *tiempo_str){
	time_t t = time(NULL);
	struct tm tm_info;
	localtime_r(&t, &tm_info);
	strftime(tiempo_str, 20, "%H:%M:%S", &tm_info);
}

void buffer(int tipoMensaje, char tiempo[20], pid_t pid, char msj[100], int fd){
	char buffer[200];
	snprintf(buffer, sizeof(buffer), "%d|%s|%d|%s\n", tipoMensaje, tiempo, pid, msj);
	write(fd, buffer, strlen(buffer));
}

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
	int tipoMensaje;
	char tiempo_str[20];
	tiempo(tiempo_str);

	fgets(mensaje, sizeof(mensaje), stdin);
	mensaje[strcspn(mensaje, "\n")] = 0;

	if(strcmp(mensaje, "/clonar") == 0){
		pid_t clone = fork();
		if(clone == 0){
			execlp("gnome-terminal", "gnome-terminal", "--", "bash", "-c", "./usuario; exec bash", NULL);
		}
		return;
	}

	if(strcmp(mensaje, "/report") == 0){
		printf("Ingrese el PID de usuario que desea reportar:");

		fgets(mensaje, sizeof(mensaje), stdin);
		mensaje[strcspn(mensaje, "\n")] = 0;

		tipoMensaje = 2;
		buffer(tipoMensaje, tiempo_str, pid, mensaje, fd);

	} else if(strcmp(mensaje, "/exit") == 0){
		strcpy(mensaje, "Desconexión");
		tipoMensaje = 3;
		buffer(tipoMensaje, tiempo_str, pid, mensaje, fd);

		usleep(100000);

		close(fd);
		close(fo);

		exit(0);

	} else{
		tipoMensaje = 1;
		buffer(tipoMensaje, tiempo_str, pid, mensaje, fd);

	}

}


void usuario(pid_t pid){
	int fd = open("/tmp/chat_in", O_WRONLY);
	if(fd == -1){
		perror("No se ha iniciado servidor");
	return;
	}

	buffer(0, 0, pid,  0, fd);

	char fifo_out[50];
	snprintf(fifo_out, sizeof(fifo_out), "/tmp/chat_out_%d", pid);
	int fo;
	while ((fo = open(fifo_out, O_RDONLY)) == -1) {
		usleep(100000);
	}

	pid_t child = fork();
	if(child == 0){
		close(fd);
		while(1){
			leer(fo);
		}
	} else {
		close(fo);
		while(1){
			escribir(fd, fo, pid);
		}
	}
}

int main(){
	pid_t pid = getpid();
	usuario(pid);
	return 0;
}
