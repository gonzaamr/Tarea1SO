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
	mkfifo("/tmp/report", 0666);
	int fd = open("/tmp/chat_in", O_RDONLY);
	int fr = open("/tmp/report", O_WRONLY);

	char buffer[200];
	int users[100];
	int user_fds[100];
	int cantUsers = 0;

	while(1){
		ssize_t n = read(fd, buffer, sizeof(buffer)-1);
		buffer[n] = '\0';

		struct mensaje msj;
		char *token;

		token = strtok(buffer, "|");
		msj.tipoMensaje = token ? atoi(token) : -1;
		token = strtok(NULL, "|");

		if(token){
			strncpy(msj.tiempo, token, sizeof(msj.tiempo));
			msj.tiempo[sizeof(msj.tiempo)-1] = '\0';
		} else msj.tiempo[0] = 0;

		token = strtok(NULL, "|");
		msj.pid = token ? atoi(token) : -1;

		token = strtok(NULL, "|");
		if(token){
			strncpy(msj.msj, token, sizeof(msj.msj));
			msj.msj[sizeof(msj.msj)-1] = '\0';
		}else msj.msj[0] = 0;

		switch(msj.tipoMensaje){
			case 0:
				users[cantUsers] = msj.pid;

				char fifo_out[50];
				snprintf(fifo_out, sizeof(fifo_out), "/tmp/chat_out_%d", msj.pid);
				mkfifo(fifo_out, 0666);
				int fo = open(fifo_out, O_WRONLY);

				user_fds[cantUsers] = fo;
				cantUsers++;

				char nuevoUsuario[200];
				snprintf(nuevoUsuario, sizeof(nuevoUsuario), "%d|%d", msj.tipoMensaje, msj.pid);
				write(fr, nuevoUsuario, strlen(nuevoUsuario));

				printf("Ha ingresado un usario con el PID: %d\n", msj.pid);
				break;

			case 1:
				for(int i=0; i<cantUsers; i++){
					if(users[i] != msj.pid){
						char transmision[200];
						snprintf(transmision, sizeof(transmision), "%s|%d:%s", msj.tiempo, msj.pid, msj.msj);
						write(user_fds[i], transmision, strlen(transmision));

						printf("%d|%s\n", msj.pid, msj.msj);
					}
				}
				break;

			case 2:
				char report[200];
				snprintf(report, sizeof(report), "%d|%s\n", msj.tipoMensaje, msj.msj);
				write(fr, report, strlen(report));

				printf("Ha llegado un reporte para: %s\n", msj.msj);
				break;

			case 3:
				char fifo[50];
				snprintf(fifo, sizeof(fifo), "/tmp/chat_out_%d", msj.pid);
				unlink(fifo);

				printf("El usuario con el PID %d se ha desconectado\n", msj.pid);
				break;
		}
	}

	close(fd);
	for(int i = 0 ; i < cantUsers ; i++){
		close(user_fds[i]);
	}
	unlink("/tmp/report");
	unlink("/tmp/chat_in");

	return 0;
}










