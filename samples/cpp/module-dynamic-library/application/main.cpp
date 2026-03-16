#include <iostream>

import ModuleDynamicLibrary.Library;
using namespace Samples::Cpp::DynamicLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}