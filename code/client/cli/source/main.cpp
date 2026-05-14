// <copyright file="main.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <algorithm> // IWYU pragma: keep
#include <filesystem> // IWYU pragma: keep
#include <format> // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <map> // IWYU pragma: keep
#include <memory> // IWYU pragma: keep
#include <optional> // IWYU pragma: keep
#include <stdexcept> // IWYU pragma: keep
#include <string>
#include <vector>

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.View;
import Soup.SML;

using namespace Opal;
using namespace Soup::SML;

#include "program.h"

int main(int argc, char **argv) {
	std::vector<std::string> args;
	for (int i = 0; i < argc; i++) {
		args.push_back(argv[i]);
	}

	auto program = Soup::Client::Program();
	return program.Run(std::move(args));
}
