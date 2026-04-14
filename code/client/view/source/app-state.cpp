// <copyright file="app-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <string>
#include <vector>

export module Soup.View:AppState;

namespace Soup::View
{
	export struct AppState
	{
		std::vector<std::string> PackagesList;
		int PackagesListSelected;

		std::vector<std::string> PackageTabList;
		int PackageTabSelected;
	};
}
