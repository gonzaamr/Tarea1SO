#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

struct Reportes {
	pid_t pid;
	int cantidad;
};

int main() {
	int fr = open("/tmp/report", O_RDONLY);
	if (fr == -1) {
		perror("No se pudo abrir /tmp/report");
		return 1;
	}

	char buffer[200];
	struct  Reportes registro[100];
	int cantUsers = 0;

	while (1) {
		ssize_t n = read(fr, buffer, sizeof(buffer) - 1);
		if (n <= 0) {
			usleep(100000);
			continue;
		}
		buffer[n] = '\0';

		char *token;

		token = strtok(buffer, "|");
		int tipoMensaje = token ? atoi(token) : -1;

		token = strtok(NULL, "|");
		int pid = token ? atoi(token) : -1;

		switch (tipoMensaje) {
			case 0:
				registro[cantUsers].pid = pid;
				registro[cantUsers].cantidad = 0;
				cantUsers++;

				printf("Usuario registrado: %d\n", pid);
				break;
			case 2:
				for(int i = 0 ; i < cantUsers ; i++){
					if(registro[i].pid == pid){
						if(registro[i].cantidad >= 9){
							kill(pid, SIGTERM);

							printf("Usuario %d ha sido baneado", pid);
							fflush(stdout);
						} else{
							registro[i].cantidad++;

							printf("El usuario %d lleva %d reportes\n", registro[i].pid, registro[i].cantidad);
						}
					} else{
						printf("No existe el PID %d\n", pid);
					}
				}
				break;
		}
	}
	return 0;
}
