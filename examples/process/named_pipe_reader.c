#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "./myfifo"

int main()
{
		int fd;
		char buf[256];
		int n;

		printf("waiting for the writer...\n");
		/* open, read, and display the message from the FIFO */
		fd = open(FIFO_NAME, O_RDONLY);
		while(1) {
				n = read(fd, buf, 256);
				printf("Received: %s", buf);
				if(strncmp(buf, "quit", 4) == 0) break;
				sleep(1);
		}
		close(fd);

		return 0;
}

