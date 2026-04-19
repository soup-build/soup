// <copyright file="tui.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

export module Soup.View:TUI;

import ftxui;
import Soup.Core;
import :AppState;
import :CustomStyle;
import :PackageLoadState;
import :TreeValue;
import :TreeView;

namespace Soup::View
{
	/// <summary>
	/// Text/Terminal UI
	/// </summary>
	export class TUI
	{
	private:
		AppState _state;

	public:
		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		void Run(Core::PackageProvider& packageProvider)
		{
			auto fileSystemState = Core::FileSystemState();

			_state = AppState();
			InitializeState(packageProvider);

			auto app = ftxui::App::Fullscreen();

			auto asciiArt = AppAsciiArt(&_state.ShowAsciiArt);

			auto packagesMenu = CreateSingleItemMenu(_state.PackagesList, &_state.PackagesListSelected);

			auto tabComponents = CreateAllPackageTabs(fileSystemState, packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&_state.PackagesListSelected);

			auto packagesView = ftxui::Container::Horizontal({
				packagesMenu,
				packagesPropertiesView,
			});

			auto appView = ftxui::Renderer(packagesView, [&] {
				// Allow for small screens to use optimal space
				// Hide pretty artwork
				_state.ShowAsciiArt = app.dimy() > 30;

				return ftxui::vbox({
					asciiArt->Render(),
					ftxui::hbox({
						packagesMenu->Render(),
						ftxui::separator(),
						packagesPropertiesView->Render() | ftxui::flex,
					}) |
					ftxui::border |
					ftxui::flex
				});
			});

			app.Loop(appView);
		}

	private:
		void InitializeGraph(
			Core::PackageProvider& packageProvider,
			int packageId)
		{
			_state.ShowAsciiArt = true;

			auto& packageInfo = packageProvider.GetPackageInfo(packageId);
			_state.PackagesList.push_back(packageInfo.Name.ToString());
			_state.PackagesIdList.push_back(packageInfo.Id);

			for (auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
			{
				for (auto& dependency : dependencyTypeSet)
				{
					// Stop at the edge of the graph and ignore duplicates
					if (!dependency.IsSubGraph &&
						std::find(_state.PackagesIdList.begin(), _state.PackagesIdList.end(), dependency.PackageId) == _state.PackagesIdList.end())
					{
						InitializeGraph(packageProvider, dependency.PackageId);
					}
				}
			}
		}
		
		void InitializeState(Core::PackageProvider& packageProvider)
		{
			auto& packageGraph = packageProvider.GetRootPackageGraph();
			InitializeGraph(packageProvider, packageGraph.RootPackageId);

			_state.PackagesListSelected = 0;

			_state.PackageTabSelected = 0;
		}

		ftxui::Components CreateAllPackageTabs(
			Core::FileSystemState& fileSystemState,
			Core::PackageProvider& packageProvider)
		{
			auto rootPackageGraphId = packageProvider.GetRootPackageGraphId();

			auto tabComponents = ftxui::Components();
			for (auto& packageId : _state.PackagesIdList)
			{
				auto& packageInfo = packageProvider.GetPackageInfo(packageId);
				auto packageLoadState = LoadPackage(
					fileSystemState, packageProvider, rootPackageGraphId, packageId);

				auto properties = TreeValueTable();

				properties.emplace("Id", TreeValue(std::to_string(packageInfo.Id)));
				properties.emplace("Name", TreeValue(packageInfo.Name.ToString()));
				properties.emplace("Root", TreeValue(packageInfo.PackageRoot.ToString()));

				for (auto& [dependencyType, dependencies] : packageInfo.Dependencies)
				{
					auto dependencyItems = TreeValueList();
					for (auto& dependency : dependencies)
					{
						dependencyItems.push_back(TreeValue(dependency.OriginalReference.ToString()));
					}

					properties.emplace(dependencyType, TreeValue(std::move(dependencyItems)));
				}

				auto propertiesList = TreeView(std::move(properties));

				// Wrap the menu in a renderer to add a frame and scroll indicator
				auto rendererPropertiesList = ftxui::Renderer(propertiesList, [propertiesList] {
					return propertiesList->Render() | ftxui::vscroll_indicator | ftxui::frame;
				});

				auto packageTabList = std::vector<std::string>({
					"Properties",
				});
				auto packageTabComponents = ftxui::Components({
					rendererPropertiesList,
				});

				auto hasPreprocessor = packageLoadState.GeneratePhase1Result.has_value() && 
					packageLoadState.GeneratePhase1Result.value().HasPreprocessor();

				if (packageLoadState.GeneratePhase1Info.has_value())
				{
					auto& generatePhase1Info = packageLoadState.GeneratePhase1Info.value();
					auto findRuntimeOrderResult = generatePhase1Info.find("RuntimeOrder");
					if (findRuntimeOrderResult == generatePhase1Info.end())
					{
						throw std::runtime_error("Generate Info Table missing RuntimeOrder List");
					}

					auto tasksComponents = std::vector<std::string>();
					for (auto& taskName : findRuntimeOrderResult->second.AsList())
					{
						tasksComponents.push_back(taskName.AsString());
					}

					auto tasksList = CreateSingleItemMenu(std::move(tasksComponents), 0);
					
					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessor Tasks");
					}
					else
					{
						packageTabList.push_back("Tasks");
					}

					packageTabComponents.push_back(std::move(tasksList));
				}

				if (packageLoadState.GeneratePhase1Result.has_value())
				{
					auto operationComponents = std::vector<std::string>();
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase1Result.value().GetGraph().GetOperations())
					{
						operationComponents.push_back(operation.Title);
					}

					auto operationsList = CreateSingleItemMenu(std::move(operationComponents), 0);
					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessors");
					}
					else
					{
						packageTabList.push_back("Operations");
					}

					packageTabComponents.push_back(std::move(operationsList));
				}


				if (packageLoadState.GeneratePhase2Info.has_value())
				{
					auto& generatePhase2Info = packageLoadState.GeneratePhase2Info.value();
					auto findRuntimeOrderResult = generatePhase2Info.find("RuntimeOrder");
					if (findRuntimeOrderResult == generatePhase2Info.end())
					{
						throw std::runtime_error("Generate Info Table missing RuntimeOrder List");
					}

					auto tasksComponents = std::vector<std::string>();
					for (auto& taskName : findRuntimeOrderResult->second.AsList())
					{
						tasksComponents.push_back(taskName.AsString());
					}

					auto tasksList = CreateSingleItemMenu(std::move(tasksComponents), 0);

					packageTabList.push_back("Tasks");
					packageTabComponents.push_back(std::move(tasksList));
				}

				if (packageLoadState.GeneratePhase2Result.has_value())
				{
					auto operationComponents = std::vector<std::string>();
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase2Result.value().GetOperations())
					{
						operationComponents.push_back(operation.Title);
					}

					auto operationsList = CreateSingleItemMenu(std::move(operationComponents), 0);
					
					packageTabList.push_back("Operations");
					packageTabComponents.push_back(std::move(operationsList));
				}

				auto tab_toggle = CustomToggle(std::move(packageTabList), &_state.PackageTabSelected);

				auto tab_container = ftxui::Container::Tab(
					std::move(packageTabComponents),
					&_state.PackageTabSelected);

				auto container = ftxui::Container::Vertical({
					tab_toggle,
					tab_container,
				});

				auto renderer = ftxui::Renderer(container, [tab_toggle, tab_container] {
					return ftxui::vbox({
							tab_toggle->Render(),
							ftxui::separator(),
							tab_container->Render(),
						});
				});

				tabComponents.push_back(renderer);
			}

			return tabComponents;
		}
	};
}
