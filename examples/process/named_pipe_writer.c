#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME "./myfifo"
int main()
{
		int fd;
		char buf[256];

		/* create the FIFO (named pipe) */
		mkfifo(FIFO_NAME, 0666);
		
		printf("waiting for the reader....\n");
		fd = open(FIFO_NAME, O_WRONLY);

		while(1) {
				printf("Enter a message: ");
				fgets(buf, 256, stdin);
				write(fd, buf, strlen(buf)+1);
				if(strncmp(buf, "quit", 4) == 0) break;
		}
		close(fd);

		/* remove the FIFO */
		unlink(FIFO_NAME);

		return 0;
}

