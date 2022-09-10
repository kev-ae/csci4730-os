#include <stdio.h>

int main()
{
		int i = 0;

		for(i=0; i < 5; i++)
		while(1)
		{
				printf("pid = %d, ppid = %d\n", getpid(), getppid());
				sleep(1);
		}
		return 0;
}

