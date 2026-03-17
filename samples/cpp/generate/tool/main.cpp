#include <iostream>
#include <fstream>

std::string_view GenerateContent()
{
	// Maybe do something more interesting here
	return R"(module;

// Include all standard library headers in the global module
#include <string>

export module Sample.Generate;

export class Helper
{
public:
	static std::string_view GetName()
	{
		return "Soup";
	}
};)";
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
	auto genFile = std::ofstream(file);

	if (!genFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return 1;
	}

	genFile << GenerateContent() << std::endl;

	genFile.close();

	return 0;
}