#include <iostream>

import Sample.StaticLibrary.Library;
using namespace Sample::StaticLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}