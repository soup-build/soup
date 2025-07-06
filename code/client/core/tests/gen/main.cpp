#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <map>
#include <optional>
#include <unordered_map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.Core.Tests;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::Test;

#include "package/package-manager-tests.gen.h"

#include "recipe/package-identifier-tests.gen.h"
#include "recipe/package-name-tests.gen.h"
#include "recipe/package-reference-tests.gen.h"
#include "recipe/recipe-extensions-tests.gen.h"
#include "recipe/recipe-tests.gen.h"
#include "recipe/recipe-sml-tests.gen.h"

#include "value-table/value-table-manager-tests.gen.h"
#include "value-table/value-table-reader-tests.gen.h"
#include "value-table/value-table-writer-tests.gen.h"

int main()
{
	std::cout << "Running Tests..." << std::endl;

	TestState state = { 0, 0 };

	state += RunBuildEngineTests();
	state += RunBuildEvaluateEngineTests();
	state += RunBuildHistoryCheckerTests();
	state += RunBuildLoadEngineTests();
	state += RunBuildRunnerTests();
	state += RunFileSystemStateTests();
	state += RunPackageProviderTests();
	state += RunRecipeBuildLocationManagerTests();

	state += RunLocalUserConfigExtensionsTests();
	state += RunLocalUserConfigTests();

	state += RunOperationGraphTests();
	state += RunOperationGraphManagerTests();
	state += RunOperationGraphReaderTests();
	state += RunOperationGraphWriterTests();
	state += RunOperationResultsTests();
	state += RunOperationResultsManagerTests();
	state += RunOperationResultsReaderTests();
	state += RunOperationResultsWriterTests();

	state += RunPackageManagerTests();

	state += RunPackageIdentifierTests();
	state += RunPackageNameTests();
	state += RunPackageReferenceTests();
	state += RunRecipeExtensionsTests();
	state += RunRecipeTests();
	state += RunRecipeSMLTests();

	state += RunValueTableManagerTests();
	state += RunValueTableReaderTests();
	state += RunValueTableWriterTests();

	std::cout << state.PassCount << " PASSED." << std::endl;
	std::cout << state.FailCount << " FAILED." << std::endl;

	if (state.FailCount > 0)
		return 1;
	else
		return 0;
}