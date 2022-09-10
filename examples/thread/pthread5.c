#include <stdio.h>
#include <pthread.h>

int counter = 0;

void *mythread(void *arg)
{
		int i;

		for (i = 0; i < 10000; i++) {
				//counter++;
				__sync_fetch_and_add( &counter, 1 );
		}

		return NULL;
}

int main(int argc, char *argv[])
{
		pthread_t p1, p2;
		printf("main: begin (counter = %d)\n", counter);
		pthread_create(&p1, NULL, mythread, "A");
		pthread_create(&p2, NULL, mythread, "B");

		// join waits for the threads to finish
		pthread_join(p1, NULL);
		pthread_join(p2, NULL);
		printf("main: done with both (counter = %d)\n", counter);
		return 0;
}
