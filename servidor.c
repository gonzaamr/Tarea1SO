#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct mensaje{
	int tipoMensaje;
	char tiempo[20];
	pid_t pid;
	char msj[100];
};

int main(){
	mkfifo("/tmp/chat_in", 0666);
	int fd = open("/tmp/chat_in", O_RDONLY);
	printf("Servidor: FIFO chat_in abierto, fd=%d\n", fd);

	char buffer[200];
	int users[100];
	int user_fds[100];
	int cantUsers = 0;

	while(1){
		ssize_t n = read(fd, buffer, sizeof(buffer)-1);
		if(n <= 0){
			printf("Servidor: read fallo o 0 bytes leidos, n=%zd\n", n);
			continue;
		}
		buffer[n] = '\0';
		printf("Servidor: Mensaje recibido crudo: '%s'\n", buffer);

		struct mensaje msj;
		char *token;

		token = strtok(buffer, "|");
		msj.tipoMensaje = token ? atoi(token) : -1;
		printf("Servidor: tipoMensaje=%d\n", msj.tipoMensaje);

		token = strtok(NULL, "|");
		if(token){
			strncpy(msj.tiempo, token, sizeof(msj.tiempo));
			msj.tiempo[sizeof(msj.tiempo)-1] = '\0';
		} else msj.tiempo[0] = 0;
		printf("Servidor: tiempo='%s'\n", msj.tiempo);

		token = strtok(NULL, "|");
		msj.pid = token ? atoi(token) : -1;
		printf("Servidor: pid=%d\n", msj.pid);

		token = strtok(NULL, "|");
		if(token){
			strncpy(msj.msj, token, sizeof(msj.msj));
			msj.msj[sizeof(msj.msj)-1] = '\0';
		} else msj.msj[0] = 0;
		printf("Servidor: msj='%s'\n", msj.msj);

		switch(msj.tipoMensaje){
			case 0:
				users[cantUsers] = msj.pid;
				char fifo_out[50];
				snprintf(fifo_out, sizeof(fifo_out), "/tmp/chat_out_%d", msj.pid);
				printf("Servidor: creando FIFO salida para pid=%d -> %s\n", msj.pid, fifo_out);
				mkfifo(fifo_out, 0666);
				int fo  = open(fifo_out, O_WRONLY);
				printf("Servidor: fd fifo_out=%d\n", fo);
				user_fds[cantUsers] = fo;
				cantUsers++;
				printf("Servidor: nuevo usuario registrado, cantUsers=%d\n", cantUsers);
				break;

			case 1:
				printf("Servidor: reenviando mensaje de pid=%d\n", msj.pid);
				for(int i=0; i<cantUsers; i++){
					if(users[i] != msj.pid){
						char transmision[200];
						snprintf(transmision, sizeof(transmision), "%s|%d:%s", msj.tiempo, msj.pid, msj.msj);
						int w = write(user_fds[i], transmision, strlen(transmision));
						printf("Servidor: write a pid=%d, w=%d\n", users[i], w);
					}
				}
				break;

			default:
				printf("Servidor: tipoMensaje desconocido=%d\n", msj.tipoMensaje);
		}
	}

	close(fd);
	for(int i = 0 ; i < cantUsers ; i++){
		close(user_fds[i]);
	}
	return 0;
}


