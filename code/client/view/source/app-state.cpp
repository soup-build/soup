// <copyright file="app-state.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <string>
#include <vector>

export module Soup.View:AppState;
import :GraphLayout;

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

		Graph PackagesGraph;
		
		// 0 show list 1 show graph
		int ShowPackagesGraphView;
		int ShowTasksGraphView;
		int ShowOperationsGraphView;

		std::vector<int> PackagesIdList;
		std::vector<std::string> PackagesList;
		std::vector<std::string> PackagesNameList;
		std::vector<PackageState> PackagesState;
		int PackagesListSelected;

		int PackageTabSelected;
	};
}
