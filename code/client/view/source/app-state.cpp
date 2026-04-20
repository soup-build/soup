// <copyright file="app-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <string>
#include <vector>

export module Soup.View:AppState;

namespace Soup::View
{
	export struct PackageState
	{
		int SelectedPreprocessorTask;
		int SelectedTask;
		int SelectedPreprocessor;
		int SelectedOperation;
	};

	export struct AppState
	{
		bool ShowAsciiArt;

		std::vector<int> PackagesIdList;
		std::vector<std::string> PackagesList;
		std::vector<PackageState> PackagesState;
		int PackagesListSelected;

		int PackageTabSelected;
	};
}
