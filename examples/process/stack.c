#include <stdio.h>

int func_b(int a, int b)
{
		printf("func_b started. a = %d, b = %d\n");

		return 0;
}

int func_a(int a)
{
		printf("func_a started. a = %d\n");

		func_b(a+10, a+20);
		return 0;
}

int main()
{
		int a = 10;
		printf("main started.\n");

		func_a(a);
		return 0;
}

