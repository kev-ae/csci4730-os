#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <syscall.h>

int global = 0;

void *mythread(void *arg) {
		printf("thread %s: ppid %d, pid %d, tid %d\n", (char*)arg, getppid(), getpid(), syscall(186));
		printf("thread %s: pthread_self(): %u\n", (char*) arg,  pthread_self());

		return NULL;
}

int main() {
		pthread_t p1, p2;
		int rc;

		printf("main thread: ppid %d, pid %d, tid %d\n", getppid(), getpid(), syscall(186));
		rc = pthread_create(&p1, NULL, mythread, "A"); assert(rc == 0);
		printf("main thread: created thread id: : %u\n", p1);
		rc = pthread_create(&p2, NULL, mythread, "B"); assert(rc == 0);
		printf("main thread: created thread id: : %u\n", p2);

		// join waits for the threads to finish
		rc = pthread_join(p1, NULL); assert(rc == 0);
		rc = pthread_join(p2, NULL); assert(rc == 0);

		printf("main: end, global = %d\n", global);
		return 0;
}
