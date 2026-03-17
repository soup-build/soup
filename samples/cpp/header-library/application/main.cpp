#include <iostream>
#include <library.h>

using namespace Sample::HeaderLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}