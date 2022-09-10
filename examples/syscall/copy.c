#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 4096
#define FNAME_LENGTH 128

int main()
{
		char ch, source_file[FNAME_LENGTH], target_file[FNAME_LENGTH];
		char buf[BUF_SIZE];
		int n;
		FILE *source, *target;


		// Source file
		printf("Enter a source filename:\n");
		fgets(source_file, FNAME_LENGTH, stdin);
		source_file[strlen(source_file) - 1] = '\0';

		source = fopen(source_file, "r");
		if( source == NULL ) exit(EXIT_FAILURE);


		// Target file
		printf("Enter a target filename:\n");
		fgets(target_file, FNAME_LENGTH, stdin);
		target_file[strlen(target_file) - 1] = '\0';

		target = fopen(target_file, "w");
		if( target == NULL ) exit(EXIT_FAILURE);


		// Copy
		while(!feof(source)) {
				n = fread(buf, 1, BUF_SIZE, source);
				fwrite(buf, 1, n, target);
				fflush(target);
		}


		//Finalize
		fclose(source);
		fclose(target);
		printf("File copied successfully.\n");

		return 0;
}

