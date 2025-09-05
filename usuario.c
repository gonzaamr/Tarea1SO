#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

int main(){
	pid_t pid = getpid();
	int fd = open("/tmp/chat_in", O_WRONLY);
	char reg_msg[50];
	snprintf(reg_msg, sizeof(reg_msg), "REGISTER|%d\n", pid);
	write(fd, reg_msg, strlen(reg_msg));

	while(1){
		char msj[100];
		fgets(msj, sizeof(msj), stdin);
		msj[strcspn(msj, "\n")]=0;

		time_t t = time(NULL);
		struct tm tm_info;
		localtime_r(&t, &tm_info);
		char time_str[20];
		strftime(time_str, sizeof(time_str), "%H:%M:%S", &tm_info);

		char buffer[200];
		snprintf(buffer, sizeof(buffer), "%s %d: %s\n", time_str, pid, msj);
		write(fd, buffer, strlen(buffer));
	}
	close(fd);
	return 0;
}
