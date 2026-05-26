#include <iostream>
#include <memory>

import BSP;

int main() {
	auto inStream = std::make_unique<BSP::StdInStream>();
	auto outStream = std::make_unique<BSP::StdOutStream>();
	auto connection = BSP::Connection(std::move(inStream), std::move(outStream));

	auto testMessage = BSP::Message("1234", "dowork");
	connection.SendMessage(testMessage);

	return 0;
}
