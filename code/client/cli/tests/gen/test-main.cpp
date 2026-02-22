#include <chrono>
#include <iostream>

import Opal;
import Soup.Core;
import Soup.Test.Assert;

using namespace Opal;
using namespace Soup::Test;

#include "commands/build-command-tests.gen.h"
#include "commands/initialize-command-tests.gen.h"
#include "commands/install-command-tests.gen.h"
#include "commands/publish-command-tests.gen.h"
#include "commands/release-command-tests.gen.h"
#include "commands/version-command-tests.gen.h"

int main()
{
	std::cout << "Running Tests..." << std::endl;

	TestState state = { 0, 0 };

	state += RunBuildCommandTests();
	state += RunInitializeCommandTests();
	state += RunPublishArtifactCommandTests();
	state += RunPublishCommandTests();
	state += RunVersionCommandTests();

	// Touch stamp file to ensure incremental builds work
	// auto testFile = std::fstream("TestHarness.stamp", std::fstream::out);
	// testFile << "TOUCH";

	std::cout << state.PassCount << " PASSED." << std::endl;
	std::cout << state.FailCount << " FAILED." << std::endl;

	if (state.FailCount > 0)
		return 1;
	else
		return 0;
}
