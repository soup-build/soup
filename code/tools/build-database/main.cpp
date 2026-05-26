#include <filesystem>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>

import Opal;
import Soup.Core;
import Soup.BuildDatabase;

void PrintUsage() {
	std::cout << "build-database [path]" << std::endl;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		PrintUsage();
		return 1;
	}

	try {
		Opal::System::IFileSystem::Register(std::make_shared<Opal::System::STLFileSystem>());

		auto workingDirectory = Opal::Path::Parse(argv[1]);

		auto content = LoadBuildGraphContent(workingDirectory);

		auto buildDatabaseFile = workingDirectory + Opal::Path("./build_database.json");

		// Open the file to read from
		auto file = Opal::System::IFileSystem::Current().OpenWrite(buildDatabaseFile, false);

		// Write the contents of the build state file
		file->GetOutStream() << content;

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}
