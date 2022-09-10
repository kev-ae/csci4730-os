#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
		int pid;

		pid = fork();

		printf("pid = %d, ppid = %d\n", getpid(), getppid());
		
		if(pid > 0) {
				printf("Wait for Child process\n");
				wait(NULL);
				printf("Child process is terminated.\n");
		} else if (pid == 0) {
				if(execl("loop", NULL) == -1) printf("error!\n");
		}

		return 0;
}

