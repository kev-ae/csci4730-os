#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

		int
main(int argc, char * argv[])
{
		int pipefd[2];
		pid_t pid;
		char buf[256];
		int n;

		pipe(pipefd); // create the pipe
		pid = fork();

		if (pid == 0) // if I am the child then
		{	
				while(1) {
						n = read(pipefd[0], buf, 256);
						printf("[Child %d]: received: %s", getpid(), buf);
						if(strncmp(buf, "quit", 4) == 0) break;
				}

				close(pipefd[0]); // close the read-end of the pipe
				exit(EXIT_SUCCESS);
		}
		else // if I am the parent then
		{
				while(1) {
						printf("[Parent %d] Enter a message: ", getpid());
						fgets(buf,256, stdin);
						write(pipefd[1], buf, strlen(buf)+1);
						if(strncmp(buf, "quit", 4) == 0) break;
						usleep(100);
				}
				wait(NULL); // wait for the child process to exit
				close(pipefd[1]); // close the write-end of the pipe, thus sending EOF to the reader
				exit(EXIT_SUCCESS);
		}
		return 0;
}
