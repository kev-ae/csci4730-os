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


void receiver(int msqid)
{
		struct msgbuf rbuf;
		msgrcv(msqid, &rbuf, MAXSIZE, 0, 0);
		printf("pid[%d] received: %s", getpid(), rbuf.mtext);
}

void sender(int msqid)
{
		struct msgbuf sbuf;
		sbuf.mtype = 1;
		printf("pid[%d]: Enter a message to add to message queue : ", getpid());
		fgets(sbuf.mtext, 128, stdin);
		msgsnd(msqid, &sbuf, strlen(sbuf.mtext)+1, 0);
}

int main()
{
		int msqid;
		int pid;
		
		if ((msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666)) < 0)
		{
				perror("msgget");
				return -1;
		}

		pid = fork();

		if(pid == 0) {
				sender(msqid);
		} else {
				wait(NULL);
				receiver(msqid);
		}

		return 0;
}

