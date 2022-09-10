#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <syscall.h>
#include <sys/types.h>

int global = 0;

void *mythread(void *arg) {
		int local  = 0;
		int i;

		printf("[Thread %d: %d]: %s\n", syscall(186), syscall(186) - getpid(), (char *) arg);

		for (i = 0; i < 15000; i++) {
				global++;
//				sleep(1);
		}
		return NULL;
}

int main() {
		pthread_t p1, p2;
		int rc;

		printf("[%d] main: begin\n", getpid());
		rc = pthread_create(&p1, NULL, mythread, "A"); assert(rc == 0);
		rc = pthread_create(&p2, NULL, mythread, "B"); assert(rc == 0);

		// join waits for the threads to finish
		rc = pthread_join(p1, NULL); assert(rc == 0);
		rc = pthread_join(p2, NULL); assert(rc == 0);

		printf("main: end, global = %d\n", global);
		return 0;
}
