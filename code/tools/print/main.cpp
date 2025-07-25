#include <iostream>
#include <filesystem>
#include <sstream>
#include <unordered_set>
#include <vector>

import Opal;
import Soup.Core;
import PrintGraph;
import PrintResults;
import PrintValueTable;

void PrintUsage()
{
	std::cout << "print [path]" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		PrintUsage();
		return 1;
	}

	try
	{
		Opal::System::IFileSystem::Register(std::make_shared<Opal::System::STLFileSystem>());

		auto file = Opal::Path::Parse(argv[1]);

		if (file.GetFileExtension() == ".bog")
		{
			PrintGraph::LoadAndPrintOperationGraph(file);
		}
		else if (file.GetFileExtension() == ".bor")
		{
			PrintResults::LoadAndPrintOperationResults(file);
		}
		else if (file.GetFileExtension() == ".bvt")
		{
			PrintValueTable::LoadAndPrintValueTable(file);
		}
		else
		{
			std::cout << "Unknown file type " << file.GetFileExtension() << std::endl;
			return 3;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}