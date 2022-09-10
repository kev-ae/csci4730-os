#include <stdio.h>
#include <unistd.h>


int GNUM = 0;
int loop()
{
		int i;
		
		for(i=0; i < 10; i++)
		{
				printf("pid = %d, ppid = %d, g = %d\n", getpid(), getppid(), GNUM++);
				sleep(1);
		}

		return 0;
}

int main()
{
		int pid;
		
		pid = fork();
		
  // 1. parent: pid <- pid of the child process
		// 2. child: pid <- 0
		// 3. error: pid <- -1
		
		printf("pid = %d, ppid = %d, returned pid = %d\n", getpid(), getppid(), pid);


		if(pid > 0) {
				printf("Wait for the child process\n");
				wait(NULL);

		} else if(pid == 0){
				loop();
		}
		return 0;
}

