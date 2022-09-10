#include <stdio.h>
#include <unistd.h>

int main()
{
		int pid;

		pid = fork();
		
		if(pid == 0){
				execv("./loop", NULL);
		} else {
				printf("parent process (%d): wait for the child.\n", getpid());
				wait(NULL);
		}

		return 0;
}

