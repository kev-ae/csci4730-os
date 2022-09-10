#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void loop()
{
		int i;

		for(i = 0; i < 10; i++)
		{
				printf("pid = %d, ppid = %d\n", getpid(), getppid());
				sleep(1);

				i = i/0;
		}
}

int main()
{
		int pid;
		int wstatus;

		pid = fork();

		printf("pid = %d, ppid = %d, returned pid = %d\n", getpid(), getppid(), pid);
		if(pid < 0) return 0;

		if(pid == 0){
				loop();
		} else {
				printf("parent process (%d): wait for the child.\n", getpid());
				wait(&wstatus);
				if(WIFEXITED(wstatus)) printf("Child process terminated normally. The Exit status: %d\n", WEXITSTATUS(wstatus));
				if(WIFSIGNALED(wstatus)) printf("Child process terminated by a signal %d\n", WTERMSIG(wstatus));
		}

		return 111; // == exit(111);
}

