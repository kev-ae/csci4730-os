#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSZ     27

int main()
{
		char c;
		int shmid;
		key_t key;
		char *shm, *s;
		char buf[256];

		// Unique identifier
		key = 0x5678;

		if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
				perror("shmget");
				exit(1);
		}
		//Attach the segment to our data space.
		if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
				perror("shmat");
				exit(1);
		}

		printf("shmid 0x%x, shm addr %p\n", shmid, shm);

		int i = 0;
		while(1) {
				printf("Enter a message: ");
				fgets(shm, 25, stdin);
				printf("Send Msg: %s\n", shm);
				*(shm+25) = '\0';
				if(strncmp(shm, "quit", 4) == 0) break;
				/*
					* Wait until the other process changes 
					* the first character of our memory to '*'.
					*/
				while (*(shm+25) != '*' && *(shm+26) != '&')
						usleep(100);
		}
		return 0;
}
