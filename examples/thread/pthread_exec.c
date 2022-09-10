#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

void *thread1(void *arg) {
		int i = 0;
		for(i = 0; i < 5; i++) {
				printf("[%d]: %s\n", syscall(SYS_gettid)-getpid(), (char *) arg);
				sleep(1);
		}
		return NULL;
}

void *thread2(void *arg) {
		int i = 0;
		for(i = 0; i < 5; i++) {
				printf("[%d]: %s\n", syscall(SYS_gettid)-getpid(), (char *) arg);
				sleep(1);
				if(i == 2) execl("/bin/ls", "", NULL);
		}
		return NULL;
}

int main() {
		pthread_t p1, p2;
		int rc;

		printf("[%d] main: begin\n", getpid());
		rc = pthread_create(&p1, NULL, thread1, "A"); assert(rc == 0);
		rc = pthread_create(&p2, NULL, thread2, "B"); assert(rc == 0);
		// join waits for the threads to finish

		//sleep(3); execl("/bin/ls", "", NULL);
				
		rc = pthread_join(p1, NULL); assert(rc == 0);
		rc = pthread_join(p2, NULL); assert(rc == 0);
		printf("[%d] main: end\n", getpid());
		return 0;
}
