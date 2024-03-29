#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <syscall.h>

int global = 0;
__thread int tls = 0;

void *mythread(void *arg) {
		int local = 0;
		//tls = rand() % 100;
		
		printf("[%d]: mythread: Address: global-%p, tls-%p, local-%p\n", 
				syscall(186)-getpid(), &global, &tls, &local);
		printf("[%d]: mythread: Value: global-%d, tls-%d, local-%d\n\n", 
				syscall(186)-getpid(), ++global, ++tls, ++local);

		return NULL;
}

int main() {
		pthread_t p1, p2;
		int rc;
		
		tls = 100;
		global = 100;

		printf("[%d] main: begin, tls = %d, global = %d\n\n", getpid(), tls, global);
		rc = pthread_create(&p1, NULL, mythread, "A"); assert(rc == 0);
		rc = pthread_create(&p2, NULL, mythread, "B"); assert(rc == 0);
				
		rc = pthread_join(p1, NULL); assert(rc == 0);
		rc = pthread_join(p2, NULL); assert(rc == 0);
		printf("[%d] main: end, tls = %d, global = %d\n", getpid(), tls, global);
		return 0;
}
