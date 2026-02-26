#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__)
#else
#error "Unknown platform"
#endif

#include <filesystem>
#include <iostream>
#include <string_view>
#include <string>

void PrintUsage()
{
	std::cout << "copy [source] [destination]" << std::endl;
}

#if defined(_WIN32)
void CopyFile(std::string_view sourcePath, std::string_view destinationPath)
{
	if (!CopyFileA(sourcePath.data(), destinationPath.data(), false))
	{
		auto errorCode = GetLastError();
		if (errorCode == ERROR_FILE_NOT_FOUND)
		{
			throw std::runtime_error("File does not exist");
		}
		else
		{
			throw std::runtime_error(
				std::format("Copy failed: {}", errorCode));
		}
	}
}
#elif defined(__linux__)
void CopyFile(std::string_view sourcePath, std::string_view destinationPath)
{
	// Get the original permissions
    std::error_code error;
    auto status = std::filesystem::status(sourcePath, error);
    if (error)
	{
        std::cerr << "Error getting status: " << error.message() << std::endl;
		throw std::runtime_error("Get status failed");
    }

    auto sourcePermissions = status.permissions();

	auto options = std::filesystem::copy_options::overwrite_existing;
	std::filesystem::copy_file(sourcePath, destinationPath, options, error);
	if (error)
	{
        std::cerr << "Error copy file: " << error.message() << std::endl;
		throw std::runtime_error("Copy failed");
	}

	std::filesystem::permissions(destinationPath, sourcePermissions, error);
    if (error)
	{
        std::cerr << "Error setting permissions: " << error.message() << std::endl;
		throw std::runtime_error("Set permissions failed");
    }
}
#else
#error "Unknown platform"
#endif

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		PrintUsage();
		return 1;
	}

	try
	{
		auto sourcePath = std::string_view(argv[1]);
		auto destinationPath = std::string_view(argv[2]);
		CopyFile(sourcePath, destinationPath);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}

	return 0;
}