#include <stdio.h>

int main()
{
		int pid;
		printf("Before fork!\n");

		pid = fork();
		if(pid == 0) 
				printf("Hello from the Child. ret = %d\n", pid);
		else
				printf("Hello from the Parent. child pid = %d\n", pid);

		return 0;
}

