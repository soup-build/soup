#include <iostream>

import Samples.Cpp.ModuleDynamicLibrary.Library;
using namespace Samples::Cpp::DynamicLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}