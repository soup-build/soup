#include <stdio.h>

int main()
{
#ifdef SPECIAL_BUILD
	printf("Hello World, Soup Style!\n");
#else
	printf("Hello World..n");
#endif
	return 0;
}