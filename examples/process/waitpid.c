#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 10

int main()
{
		int pid[N];
		int wstatus;
		int i;

		for(i = 0; i < N; i++) 
		{
				pid[i] = fork();
				if(pid[i] < 0) {
						printf("fork failed. %d\n", i);
						return 0;
				}

				if(pid[i] == 0) {
						printf("[%d] Child: pid %d\n", i, getpid());
						sleep(3);
						return i;
				}
		}

		for(i = 0; i < N; i++)
		{
				waitpid(pid[i], &wstatus, 0);
				printf("child [%d] ", i);
				if(WIFEXITED(wstatus)) printf("terminated normally. The Exit status %d\n", WEXITSTATUS(wstatus));
				if(WIFSIGNALED(wstatus)) printf("terminated by a signal %d\n", WTERMSIG(wstatus));
		}

		return 0;
}

