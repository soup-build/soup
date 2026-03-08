#include <iostream>
#include <filesystem>

std::string_view GenerateContent()
{
	// Maybe do something more interesting here
	return "module;

// Include all standard library headers in the global module
#include <string>

export module Samples.Cpp.GenerateFile;

export class Helper
{
public:
	static std::string GetName()
	{
		return \"Soup\";
	}
};";
}

void PrintUsage()
{
	std::cout << "gen [path]" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		PrintUsage();
		return 1;
	}

	auto file = argv[1];
	std::ofstream genFile(file);

	if (!genFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 1;
	}

	genFile << GenerateContent() << std::endl;

	genFile.close();

	return 0;
}