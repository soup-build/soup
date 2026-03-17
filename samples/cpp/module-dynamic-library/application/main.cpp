#include <iostream>

import Sample.ModuleDynamicLibrary.Library;
using namespace Sample::ModuleDynamicLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}