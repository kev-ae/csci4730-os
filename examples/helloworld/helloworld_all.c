#include <stdio.h>

int main()
{
		char buf[16] = "Hello World\n";

		printf("Hello World\n");
		fprintf(stdout, "Hello World\n");
		write(1, buf, 16);
		fwrite(buf, 16, 1, stdout);
		
		return 0;
}

