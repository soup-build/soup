// <copyright file="tui.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <vector>

export module Soup.View:TUI;

import ftxui;
import Soup.Core;
import :AppState;
import :CustomStyle;
import :PackageLoadState;

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

			auto packagesMenu = CreateSingleItemMenu(_state.PackagesList, &_state.PackagesListSelected);

			auto tabComponents = CreateAllPackageTabs(fileSystemState, packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&_state.PackagesListSelected);

			auto rendererProperties = ftxui::Renderer(packagesPropertiesView, [&] {
				return packagesPropertiesView->Render();
			});

			auto packagesView = ftxui::Container::Horizontal({
				packagesMenu,
				rendererProperties,
			});

			auto packagesViewRenderer = ftxui::Renderer(packagesView, [&] {
				return ftxui::hbox({
					packagesMenu->Render(),
					ftxui::separator(),
					rendererProperties->Render() | ftxui::flex,
				}) |
				ftxui::border |
				ftxui::flex;
			});

			auto appAsciiArt = AppAsciiArt();

			auto appView = ftxui::Container::Vertical({
				appAsciiArt,
				packagesViewRenderer,
			});

			app.Loop(appView);
		}

	private:
		void InitializeGraph(
			Core::PackageProvider& packageProvider,
			int packageId)
		{
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

				auto properties = ftxui::Components();

				properties.push_back(CreateSingleItemMenuEntry(std::to_string(packageInfo.Id)));
				properties.push_back(CreateSingleItemMenuEntry(packageInfo.Name.ToString()));
				properties.push_back(CreateSingleItemMenuEntry(packageInfo.PackageRoot.ToString()));

				for (auto& [dependencyType, dependencies] : packageInfo.Dependencies)
				{
					auto dependencyItems = ftxui::Components();
					for (auto& dependency : dependencies)
					{
						dependencyItems.push_back(CreateSingleItemMenuEntry(dependency.OriginalReference.ToString()));
					}

					properties.push_back(ftxui::Collapsible(dependencyType, Inner(dependencyItems)));
				}

				auto propertiesList = ftxui::Container::Vertical(std::move(properties));
				auto tasksList = CreateSingleItemMenu({
					"Tasks",
				}, 0);

				auto packageTabList = std::vector<std::string>({
					"Properties",
					"Tasks",
				});
				auto packageTabComponents = ftxui::Components({
					propertiesList,
					tasksList,
				});

				if (packageLoadState.GeneratePhase1Result.has_value())
				{
					auto operationComponents = std::vector<std::string>();
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase1Result.value().GetGraph().GetOperations())
					{
						operationComponents.push_back(operation.Title);
					}

					auto operationsList = CreateSingleItemMenu(std::move(operationComponents), 0);
					if (packageLoadState.GeneratePhase1Result.value().HasPreprocessor())
					{
						packageTabList.push_back("Preprocessors");
					}
					else
					{
						packageTabList.push_back("Operations");
					}

					packageTabComponents.push_back(std::move(operationsList));
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

				auto tab_toggle = ftxui::Toggle(packageTabList, &_state.PackageTabSelected);

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
						}) |
						ftxui::border;
				});

				tabComponents.push_back(renderer);
			}

			return tabComponents;
		}
	};
}
