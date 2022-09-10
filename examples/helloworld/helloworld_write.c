#include <stdio.h>

int main()
{
		char buf[16] = "Hello World!\n";

		write(1, buf, 16);

		return 0;

}

