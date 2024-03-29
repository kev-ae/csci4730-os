#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define BASEPID 9000
#define MAX_SIZE 1000

int matrix1[MAX_SIZE][MAX_SIZE], matrix2[MAX_SIZE][MAX_SIZE],
				result[MAX_SIZE][MAX_SIZE];

typedef struct {
		long priority;
		int index;
		int finished;
		int result[MAX_SIZE];
} matrix;

int main(int argc, char *argv[]) {
		int size = atoi(argv[1]), rows, cols, z, PID, proc, parent = 1,
						inbox, outbox, mailbox, status;
		matrix message;
		message.priority = 2;
		message.finished = 0;

		// Check matrix size
		if (size > MAX_SIZE) {
				printf("Matrix too large\n");
				exit(1);
		}

		// Generate matrices
		srand(time(NULL));
		for (rows = 0; rows < size; rows++) {
				for (cols = 0; cols < size; cols++) {
						matrix1[rows][cols] = rand();
						matrix2[rows][cols] = rand();
						result[rows][cols] = 0;
						sleep(0);
				}
		}

		// Initialize inbox
		inbox = msgget(BASEPID, 0600 | IPC_CREAT);
		if (inbox < 0) {
				printf("Could not initialize inbox");
				exit(1);
		}

		// Fire up processes
		long nprocs = 1; //sysconf(_SC_NPROCESSORS_ONLN) * 2;
		if (nprocs <= 0) {
				printf("No processors are available to this process.");
				exit(1);
		}
		int processes[nprocs], mailboxes[nprocs];
		for (proc = 0; proc < nprocs; proc++) {
				// Initialize inbox
				mailbox = msgget(BASEPID + 1 + proc, 0600 | IPC_CREAT);
				if (mailbox < 0) {
						printf("Could not initialize inbox");
						exit(1);
				}

				PID = fork();
				if (PID == 0) { // Child process
						parent = 0;
						outbox = inbox;
						inbox = mailbox;
						break;
				} else { // Parent process
						processes[proc] = PID;
						mailboxes[proc] = mailbox;
				}
		}

		// Do the matrix multiplication
		if (parent == 1) {
				// Populate queues
				for (rows = 0; rows < size; rows++) {
						message.index = rows;
						status = msgsnd(mailboxes[rows % nprocs], &message, sizeof(message) - sizeof(long), 0);
						//printf("Sent row %d to %d\n", rows, mailboxes[rows % nprocs]);
						if (status < 0) {
								perror("Could not populate queue");
								exit(1);
						}
				}

				//printf("Finished populating queues\n");

				// Empty mailbox
				for (rows = 0; rows < size; rows++) {
						status = msgrcv(inbox, &message, sizeof(message) - sizeof(long), 0, 0);
						for (cols = 0; cols < size; cols++) {
								result[message.index][cols] = message.result[cols];
						}
				}

				// Notify processes to end
				for (proc = 0; proc < nprocs; proc++) {
						message.finished = 1;
						//printf("Sending termination signal to %d\n", mailboxes[proc]);
						status = msgsnd(mailboxes[proc], &message, sizeof(message) - sizeof(long), 0);
						//printf("Successfully send termination signal to %d\n", mailboxes[proc]);
						if (status < 0) {
								perror("Could not send termination signal");
								exit(1);
						}
				}

				// Wait on children before exiting
				for (proc = 0; proc < nprocs; proc++) {
						waitpid(processes[proc]);
				}

				// Free inbox
				struct msqid_ds msqid_ds, *buf;
				buf = & msqid_ds;
				status = msgctl(inbox, IPC_RMID, buf);
				if (status < 0) {
						perror("Could not free inbox");
						exit(1);
				}

				printf("%*s%*d.0\t", 15, "fork", 15, size);
		} else {
				// Block on queue input, return resultant row
				while (1) {
						//printf("Listening on %d\n", inbox);
						fflush(stdout);
						status = msgrcv(inbox, &message, sizeof(message) - sizeof(long), 0, 0);
						if (status < 0) {
								perror("Could not receive message");
								exit(1);
						}
						if (message.finished == 1) {
								break;
						}

						memset(message.result, 0, size);
						//printf("Received message in inbox %d\n", inbox);

						for (cols = 0; cols < size; cols++) {
								for (z = 0; z < size; z++) {
										message.result[z] += matrix1[message.index][cols] * matrix2[cols][z];
								}
						}
						//printf("Sending calculated row to %d\n", outbox);
						status = msgsnd(outbox, &message, sizeof(message) - sizeof(long), 0);
						//printf("Sent calculated row\n");
						if (status < 0) {
								perror("Could not send response");
								exit(1);
						}
				}

				// Free inbox
				struct msqid_ds msqid_ds, *buf;
				buf = & msqid_ds;
				status = msgctl(inbox, IPC_RMID, buf);
				exit(0);
		}

		return 0;
}
