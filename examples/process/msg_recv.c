#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define MAXSIZE     128

struct msgbuf
{
		long mtype;
		char mtext[MAXSIZE];
};

int main()
{
		int msqid;
		key_t key;
		struct msgbuf rbuf;

		key = 1234;

		if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0)
		{
				perror("msgget()");
				return -1;
		}


		while(1) {
				msgrcv(msqid, &rbuf, MAXSIZE, 0, 0);
				printf("received: %s", rbuf.mtext);
				if(strncmp(rbuf.mtext, "quit", 4) == 0) break;

				//sleep(2);
				usleep(100);
		}

		return 0;
}

