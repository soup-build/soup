#include <iostream>

import StaticLibrary.Library;
using namespace Samples::Cpp::StaticLibrary::Library;

int main()
{
	std::cout << "Hello World, " << Helper::GetName() << " Style!" << std::endl;
	return 0;
}