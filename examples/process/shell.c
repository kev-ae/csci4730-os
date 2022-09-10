#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
		int pid;
		char buf[256];
		char *narg[] = {NULL, NULL};

		while(1) {
				printf("myshell>>");
				fgets(buf, 256, stdin);
				
				buf[strlen(buf) - 1] = '\0';

				if(strncmp(buf, "exit", 5) == 0 || strncmp(buf, "quit", 5) == 0) return 0;

				pid = fork();

				if(pid < 0) return 0;

				if(pid == 0){ // Child
						narg[0] = buf;
						if(execv(buf, narg) == -1) printf("error!\n");
						return 0;
				} else { // Parent
						wait(NULL);
				}
		}

		return 0;
}

