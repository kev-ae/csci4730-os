#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

#define SHMSZ     27

main()
{
		int shmid;
		key_t key;
		char *shm;
		char s[256];

		key = 0x5678;

		if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
				perror("shmget");
				exit(1);
		}

		if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
				perror("shmat");
				exit(1);
		}
		printf("shmid 0x%x, shm addr %p\n", shmid, shm);

		while(1) {
				while (*(shm+25) == '*')
						usleep(100);

				printf("Message received: %s\n", shm);
				if(strncmp(shm, "quit", 4) == 0) break;
				sprintf(shm+25, "*");
		}

		return 0;
}

