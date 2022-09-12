#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "matmul.h"

void serial_mat_mult();
void parallel_mat_mult(int numProc, int crashRate);

int main(int argc, char **argv) 
{
		int crashRate = 0;
		struct timeval  t_begin, t_end;

		if(argc < 2) {
				printf("usage: %s <filename> [crash rate]\n", argv[0]);
				return 0;
		}
		
		/** 1st arg: file name that contains matrices **/
		/** 2nd (optional) arg: crash rate between 0% and 30%. 
			* Each child process has that much chance to crash.
			**/
		if(argc > 2) {
				crashRate = atoi(argv[2]);
				if(crashRate < 0) crashRate = 0;
				if(crashRate > 30) crashRate = 30; 
				printf("Child processes' crash rate: %d\%\n", crashRate);
		}

		read_matrix(argv[1]);

/** Matrix multiplication **/
		gettimeofday(&t_begin, NULL);
		serial_mat_mult();
		gettimeofday(&t_end, NULL);
		
		printf("================================================\n");
		printf("Serial multiplication took %f seconds.\n",
		         (double) (t_end.tv_usec - t_begin.tv_usec) / 1000000 +
											(double) (t_end.tv_sec - t_begin.tv_sec));
		
		if(m < 10 && p < 10) {
				printf("The result from the serial calculation: \n");
				print_matrix(C_serial, m, p);
		}
		printf("================================================\n\n");


/** Multi-process matrix multiplication **/
		gettimeofday(&t_begin, NULL);
		parallel_mat_mult(m, crashRate);
		gettimeofday(&t_end, NULL);
		
		printf("================================================\n");
		printf("Parallel multiplication took %f seconds.\n",
		         (double) (t_end.tv_usec - t_begin.tv_usec) / 1000000 +
											(double) (t_end.tv_sec - t_begin.tv_sec));
		
		if(m < 10 && p < 10) {
				printf("The result from the parallel calculation: \n");
				print_matrix(C_parallel, m, p);
		}
		printf("================================================\n");

/** Compare the results **/
		if(compare_matrices(C_serial, C_parallel, m, p) == -1)
		{
				printf("** Serial and parallel results are NOT matched.**\n");
		} else {
				printf("** Serial and parallel results are matched. **\n");
		}
		printf("================================================\n");
		return 0;
}

void serial_mat_mult()
{
		int i, j, k;

		for(i = 0; i < m; i++){
				for(j = 0; j < p; j++){
						C_serial[i][j] = linear_mult(A[i], B_tran[j], n);
				}
		}
}

void child_process_core(int i, int pipefd, int crashRate)
{
		int temp, j;

		printf("The child process (pid:%d) created to calculate job #(%d/%d).\n", getpid(), i, m);
		simulate_crash(crashRate);
		
		/** Design and implement child processes function.
			* Each child process takes care of a part of the calculation.
			* Send the result to the parent via pipe. 
			**/

		// loop through B and get the entire row of the new matrix
		for(j = 0; j < p; j++) {
			temp = linear_mult(A[i], B_tran[j], n);
			write(pipefd, &temp, sizeof(temp));
		}

		// close writing pipe
		close(pipefd);
}

void parallel_mat_mult(int numProc, int crashRate)
{
		int pid[numProc];
		int pipefd[numProc][2];
		int wstatus, i, j, k;
		int runningChild = numProc;
		int buf[1];
		int status_id;
		int flag = 1;

		for(i = 0; i < numProc; i++)
		{
				pipe(pipefd[i]);
				pid[i] = fork();

				if(pid[i] == 0) {
					 child_process_core(i, pipefd[i][1], crashRate);
						exit(0);
				} else if(pid[i] < 0) {
						printf("Fork failed\n");
						exit(0);
				}
		}
		
		/** Parent process waits for the children processes.
			* Read the results from each child process via pipe, and store them into C_parallel.
			* Design and implement the crash recovery **/
		for(j = 0; j < numProc; j++) {
			while(flag) {
				waitpid(pid[j], &status_id, 0);
				if(WIFEXITED(status_id)) {
					flag = 0;	
				} else {
					pid[j] = fork();
					
					if(pid[j] == 0) {
						child_process_core(j, pipefd[j][1], crashRate);
						exit(0);
					} else if (pid[j] < 0) {
						printf("Fork failed\n");
						exit(0);
					}
				}
			}
			for(k = 0; k < p; k++) {
				read(pipefd[j][0], buf, sizeof(int));
				C_parallel[j][k] = buf[0];				
			}	
			close(pipefd[j][0]);
			flag = 1;
		}
}

