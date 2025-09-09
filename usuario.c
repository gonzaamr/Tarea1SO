#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

struct mensaje{
    int tipoMensaje;
    char tiempo[20];
    pid_t pid;
    char msj[100];
};

int main(){
    struct mensaje registro;
    pid_t pid = getpid();

    registro.tipoMensaje = 0;
    registro.pid = pid;
    registro.tiempo[0] = 0;
    registro.msj[0] = 0;

    // Abrir FIFO de entrada del servidor
    int fd = open("/tmp/chat_in", O_WRONLY);
    if(fd == -1){
        perror("Abrir fifo chat_in");
        return 1;
    }
    printf("Cliente: PID=%d, fifo chat_in abierto, fd=%d\n", pid, fd);

    // Enviar registro al servidor
    char reg_msg[50];
    snprintf(reg_msg, sizeof(reg_msg), "%d| |%d|\n", registro.tipoMensaje, registro.pid);
    int w = write(fd, reg_msg, strlen(reg_msg));
    printf("Cliente: registro enviado '%s', bytes=%d\n", reg_msg, w);

    // Fork: hijo lee mensajes, padre envía
    pid_t child = fork();

    // FIFO de salida (del servidor hacia este cliente)
	char fifo_out[50];
	snprintf(fifo_out, sizeof(fifo_out), "/tmp/chat_out_%d", pid);

	int fo;
	while ((fo = open(fifo_out, O_RDONLY)) == -1) {
		usleep(100000); // espera 0.1s
	}
	printf("Cliente: fifo_out abierto, fd=%d\n", fo);	

    if(child == 0){
        // Hijo: recibe mensajes
        char rcv_buffer[200];
        while(1){
            ssize_t n = read(fo, rcv_buffer, sizeof(rcv_buffer)-1);
            if(n > 0){
                rcv_buffer[n] = '\0';
                printf("Cliente hijo: recibido '%s'\n", rcv_buffer);
            } else {
                // Esto ayuda a ver si no llega nada
                // no bloquear CPU con sleep muy largo
                usleep(100000);
            }
        }
    } else {
        // Padre: envía mensajes
        struct mensaje msj;
        msj.pid = pid;
        msj.tipoMensaje = 1; // mensaje normal
        while(1){
            fgets(msj.msj, sizeof(msj.msj), stdin);
            msj.msj[strcspn(msj.msj, "\n")] = 0;

            time_t t = time(NULL);
            struct tm tm_info;
            localtime_r(&t, &tm_info);
            strftime(msj.tiempo, sizeof(msj.tiempo), "%H:%M:%S", &tm_info);

            char send_buffer[200];
            snprintf(send_buffer, sizeof(send_buffer), "%d|%s|%d|%s\n",
                     msj.tipoMensaje, msj.tiempo, msj.pid, msj.msj);
            int w = write(fd, send_buffer, strlen(send_buffer));
            printf("Cliente padre: enviado '%s', bytes=%d\n", send_buffer, w);
        }
    }

    close(fd);
    close(fo);
    return 0;
}
