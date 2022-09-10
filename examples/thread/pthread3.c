#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock;

void calculation()
{
		pthread_mutex_lock(&lock);
		counter = counter + 1;
		pthread_mutex_unlock(&lock);
		// extra work..
		usleep(10);
}

void *mythread(void *arg)
{
		int i;
		// Critical Section

		for (i = 0; i < 10000; i++) {
				calculation();
		}


		return NULL;
}

int main(int argc, char *argv[])
{
		int i;
		
		pthread_t p1, p2;

		if (pthread_mutex_init(&lock, NULL) != 0)
		{
				printf("mutex init failed\n");
				return 1;
		}


		printf("main: begin (counter = %d)\n", counter);
		pthread_create(&p1, NULL, mythread, "T1");
		pthread_create(&p2, NULL, mythread, "T2");

		// join waits for the threads to finish
		pthread_join(p1, NULL);
		pthread_join(p2, NULL);

		printf("main: done with both (counter = %d)\n", counter);
		return 0;
}
