#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <pthread.h>
#include <syscall.h>
#include <assert.h>

int counter = 0;

void sig_handler(int signo)
{
  printf("[pid %d, tid %d] received SIGNAL %d\n", getpid(), syscall(SYS_gettid) - getpid(), signo);
}

void register_sig_handler()
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
				printf("\ncan't catch SIGINT\n");
}

void *mythread(void *arg) {
		printf("[pid %d, tid %d]: %s\n", getpid(), syscall(SYS_gettid) - getpid(), (char *) arg);
		if((char*)arg == "A") register_sig_handler();
		while(1) sleep(1);

		return NULL;
}

int main() {
		pthread_t p1, p2;
		int rc;

		printf("[pid %d, tid %d] main: begin\n", getpid(), syscall(SYS_gettid) - getpid());
		rc = pthread_create(&p1, NULL, mythread, "A"); assert(rc == 0);
		rc = pthread_create(&p2, NULL, mythread, "B"); assert(rc == 0);

		//register_sig_handler();

		// join waits for the threads to finish
		rc = pthread_join(p1, NULL); assert(rc == 0);
		rc = pthread_join(p2, NULL); assert(rc == 0);

		printf("main: end, counter = %d\n", counter);
		return 0;
}
