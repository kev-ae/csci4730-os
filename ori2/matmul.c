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
		int wstatus, status_id, index, i, j, k;
		int runningChild = numProc;
		int buf[1];
		int pid_to_pipe[numProc];
		int proc_left = numProc;
		int end = numProc - 1;
		int fail_p = 0;

		for(i = 0; i < numProc; i++)
		{
				pipe(pipefd[i]);
				pid[i] = fork();
				pid_to_pipe[i] = i;

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
		j = 0;
		while(proc_left != 0) {
			// get status of pid and mapping from pid to pipe
			waitpid(pid[j], &status_id, 0);
			index = pid_to_pipe[j];
			if(WIFEXITED(status_id)) {
				// success, read from pipe and write to C_parallel matrix
				for(k = 0; k < p; k++) {
					read(pipefd[index][0], buf, sizeof(int));
					C_parallel[index][k] = buf[0];				
				}	

				// close pipe and decrease number of processes
				close(pipefd[index][0]);
				proc_left--;
			} else {
				// failed, create a new child process and shift towards beginning of pid array
				pid[fail_p] = fork();

				// map pipe index to new position of pid in array
				pid_to_pipe[fail_p] = index;
				
				if(pid[fail_p] == 0) {
					// child process will run calculate method
					child_process_core(index, pipefd[index][1], crashRate);
					exit(0);
				} else if (pid[fail_p] < 0) {
					printf("Fork failed\n");
					exit(0);
				}
				fail_p++;
			}
			
			/* 
 			* if counter j reaches end pointer, reset j to check on failed processes.
			* update end pointer to current processes left
			* reset head pointer back to 0 in case more processes failed
			*/
			if(j == end) {
				j = 0;
				end = proc_left - 1;
				fail_p = 0;
			} else {
				j++;
			}
		}
}

