#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "webserver.h"

#define MAX_REQUEST 10

int request[MAX_REQUEST];

sem_t sem_full;
sem_t sem_empty;
pthread_mutex_t mutex;
int head;
int tail;

void* listener(void* arg) {
    int ret;
    int sock = *(int*)arg;
    free(arg);

    // listener loop
    while (1) {
        // accept connection
        ret = accept(sock, NULL, NULL);

        if (ret < 0) break;

        // wait if queue is full
        sem_wait(&sem_empty);

        // add request to queue and update head
        request[head] = ret;
        head = (head + 1) % MAX_REQUEST;

        // tell worker that there is something in the queue
        sem_post(&sem_full);
    }
}

void* worker(void* arg) {
    int ret;

    // worker loop
    while (1) {
        // wait if queue is empty
        sem_wait(&sem_full);

        // one thread can access the queue at a time
        pthread_mutex_lock(&mutex);

        // get request and update tail
        ret = request[tail];
        tail = (tail + 1) % MAX_REQUEST;

        // unlock mutex
        pthread_mutex_unlock(&mutex);

        // open up space in queue
        sem_post(&sem_empty);

        // do the task
        process(ret);
    }
}

void multi_threaded_server(int sock, int n_thread)
{
    /* Initialize mutex lock and semaphores */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_empty, 0, MAX_REQUEST);
    sem_init(&sem_full, 0, 0);
    pthread_t listener_t;
    pthread_t worker_t[n_thread];
    int i;
    int ret;
    head = 0;
    tail = 0;
    int* sock_p = malloc(sizeof(int));
    *sock_p = sock;
		
    /* Create a listener thread and "n_thread" worker threads. */
    pthread_create(&listener_t, NULL, listener, sock_p);
    for (i = 0; i < n_thread; i++) {
        pthread_create(&worker_t[i], NULL, worker, "Worker");
    }
    
    // main thread check for failed threads and respawn them 
    i = 0;
    while (1) {
        ret = pthread_tryjoin_np(worker_t[i], NULL);
        if (ret == 0) {
            pthread_create(&worker_t[i], NULL, worker, "Worker");
        }
        i = (i + 1) % n_thread;
    }
}

void single_threaded_server(int sock)
{
		int s;

		while (1)
		{
				s = accept(sock, NULL, NULL);
				if (s < 0) break;

				process(s);
		}
}


int main(int argc, char *argv[])
{
		int i, sock;
		int n_thread;

		/* Random seed */
		srand(getpid() + time(NULL));

		/* Set # of worker thread */
		n_thread = 0;
		if(argc > 1) {
				n_thread = atoi(argv[1]);
				if(n_thread > 100) n_thread = 100;
				if(n_thread < 0) {
						fprintf(stderr, "usage: ./webserver (#_of_threads) (crash_rate(%))\n");
				}
		}

		/* Set crash rate */
		if(n_thread > 0 && argc > 2) {
				CRASH = atoi(argv[2]);
				if(CRASH > 30) CRASH = 30;
				if(CRASH < 0) {
						fprintf(stderr, "usage: ./webserver (#_of_threads) (crash_rate(%))\n");
				}
				printf("[pid %d] CRASH RATE = %d\%\n", getpid(), CRASH);
		}
			
		/* Initialize a socket */
		sock = binding_and_listen();

		/* start the server */
		if(n_thread == 0) single_threaded_server(sock);
		else multi_threaded_server(sock, n_thread);
		
		close(sock);

		return 0;
}

