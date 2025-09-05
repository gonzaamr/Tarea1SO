#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(){
	mkfifo("/tmp/chat_in", 0666);
	int  fd = open("/tmp/chat_in", O_RDONLY);
	char buffer[100];
	while(1){
		read(fd, buffer, sizeof(buffer));
		printf(buffer);
	}
	close(fd);
	return 0;
}

