#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
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
		struct msgbuf sbuf;

		key = 1234;

		if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0)
		{
				perror("msgget");
				return -1;
		}

		sbuf.mtype = 1;

		while(1)
		{
				printf("Enter a message to add to message queue : ");
				fgets(sbuf.mtext, 128, stdin);
				msgsnd(msqid, &sbuf, strlen(sbuf.mtext)+1, 0);

				if(strncmp(sbuf.mtext, "quit", 4) == 0) break;
		}
		return 0;
}

