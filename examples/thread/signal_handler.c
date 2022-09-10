#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("[%d] received SIGINT\n", getpid());
}

void register_sig_handler()
{
  if (signal(SIGINT, sig_handler) == SIG_ERR)
				printf("\ncan't catch SIGINT\n");
}

int main(void)
{
		register_sig_handler();

		printf("waiting for a signal....\n");
  while(1) 
				sleep(1);
  return 0;
}

