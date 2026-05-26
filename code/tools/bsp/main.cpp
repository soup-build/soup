#include <iostream>

import json11;

int main() {
	std::cout << "Hello World" << std::endl;

	// Parse the json
	std::string errorMessage;
	auto json = json11::Json::parse(jsonContent, errorMessage);

	return 0;
}
