#include <stdio.h>

int *g;

void fnc_b()
{
		(*g)++;
}

void fnc_a()
{
		int local_a = 10;
		g = &local_a;
		
		printf("Before fnc_b: local_a = %d\n", local_a);

		fnc_b();
		fnc_b();

		printf("After fnc_b: local_a = %d\n", local_a);
}


int main()
{
		fnc_a();

		return 0;
}

