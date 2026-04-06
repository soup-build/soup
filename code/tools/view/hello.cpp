#include <stdio.h>

extern "C"
{
	void hello()
	{
		printf("Hello from C++");
		fflush(stdout);
	}
}