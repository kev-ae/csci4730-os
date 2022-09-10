#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main()
{
		char buf[16] = "Hello World!\n";
  syscall(SYS_write, 1, buf, 16);

}

