#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int loop()
{
		int i;
		
		for(i=0; i < 20; i++)
		{
				printf("pid = %d, ppid = %d\n", getpid(), getppid());
				sleep(1);
		}

		return 0;
}

int main()
{
		int pid, pid1;
		int c_ret;

		pid = fork();

		printf("pid = %d, ppid = %d\n", getpid(), getppid());
		if(pid > 0) {
				//wait(&c_ret);
				//printf("ret %d\n", WEXITSTATUS(c_ret));
				sleep(5);
				abort();
		} else if (pid==0) {
				loop();
		}

		exit(156);
}

