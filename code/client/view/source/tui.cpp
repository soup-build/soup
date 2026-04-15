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

			auto packagesMenu = CreateSingleItemMenu(&_state.PackagesList, &_state.PackagesListSelected);

			auto tabComponents = CreateAllPackageTabs(fileSystemState, packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&_state.PackagesListSelected);

			auto rendererProperties = ftxui::Renderer(packagesPropertiesView, [&] {
				return packagesPropertiesView->Render() | ftxui::vscroll_indicator | ftxui::xflex_grow | ftxui::frame;
			});

			auto packagesView = ftxui::Container::Horizontal({
				packagesMenu,
				rendererProperties,
			});

			auto packagesViewRenderer = ftxui::Renderer(packagesView, [&] {
				return ftxui::vbox({
					AppAsciiArt(),
					ftxui::hbox({
						packagesMenu->Render(),
						ftxui::separator(),
						rendererProperties->Render(),
					}) |
					ftxui::border,
				});
			});

			app.Loop(packagesViewRenderer);
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
			
				auto packageTabList = std::vector<std::string>({
					"Properties",
					"Tasks",
					"Operations",
				});

				auto tab_toggle = ftxui::Toggle(packageTabList, &_state.PackageTabSelected);

				auto propertiesList = ftxui::Container::Vertical(std::move(properties));
				auto tasksList = ftxui::Container::Vertical({
					CreateSingleItemMenuEntry("Tasks"),
				});

				auto operationComponents = ftxui::Components();
				if (packageLoadState.GeneratePhase1Result.has_value())
				{
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase1Result.value().GetGraph().GetOperations())
					{
						operationComponents.push_back(CreateSingleItemMenuEntry(operation.Title));
					}
				}

				auto operationsList = ftxui::Container::Vertical(std::move(operationComponents));

				auto tab_container = ftxui::Container::Tab({
						propertiesList,
						tasksList,
						operationsList,
					},
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
