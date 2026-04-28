// <copyright file="tui.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
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
import :GraphView;
import :OperationsView;
import :PackageLoadState;
import :RecipeTreeConverter;
import :TasksView;
import :TreeValue;
import :TreeView;
import :ValueTreeConverter;

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

			Graph packagesGraph = {};
			InitializeState(packageProvider, packagesGraph);

			auto app = ftxui::App::Fullscreen();

			auto asciiArt = AppAsciiArt(&_state.ShowAsciiArt);

			auto packagesList = CreateSingleItemMenu(_state.PackagesList, &_state.PackagesListSelected);
			auto packagesGraphView = GraphView(std::move(packagesGraph), _state.PackagesNameList);

			auto packagesMenu = ScrollFrame(ftxui::Container::Tab({
					std::move(packagesList),
					std::move(packagesGraphView),
				},
				&_state.ShowPackagesGraphView));

			auto tabComponents = CreateAllPackageTabs(fileSystemState, packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&_state.PackagesListSelected);

			auto packagesView = ftxui::Container::Horizontal({
				packagesMenu,
				packagesPropertiesView,
			});

			auto statusBar = ftxui::Renderer([] {
				return ftxui::text("<v> Toggle Graph View | <c> Toggle Child Graph View");
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
					}) | ftxui::border | ftxui::flex,
					statusBar->Render(),
				});
			});

			appView |= ftxui::CatchEvent([&](ftxui::Event event)
			{
				if (event == ftxui::Event::v)
				{
					_state.ShowPackagesGraphView = !_state.ShowPackagesGraphView;
					return true;
				}
				else if (event == ftxui::Event::c)
				{
					_state.ShowChildGraphView = !_state.ShowChildGraphView;
					return true;
				}

				return false;
			});

			app.Loop(appView);
		}

	private:
		int InitializeGraph(
			Core::PackageProvider& packageProvider,
			Graph& packagesGraph,
			int packageId)
		{
			auto& packageInfo = packageProvider.GetPackageInfo(packageId);

			int packageIndex = _state.PackagesIdList.size();

			_state.PackagesList.push_back(packageInfo.Name.ToString());
			_state.PackagesNameList.push_back(packageInfo.Name.GetName());
			_state.PackagesIdList.push_back(packageInfo.Id);

			for (auto& [dependencyType, dependencyTypeSet] : packageInfo.Dependencies)
			{
				for (auto& dependency : dependencyTypeSet)
				{
					// Stop at the edge of the graph and ignore duplicates
					if (!dependency.IsSubGraph &&
						std::find(_state.PackagesIdList.begin(), _state.PackagesIdList.end(), dependency.PackageId) == _state.PackagesIdList.end())
					{
						auto childIndex = InitializeGraph(packageProvider, packagesGraph, dependency.PackageId);

						// Add an edge for the graph
						packagesGraph.Edges.push_back({packageIndex, childIndex});
					}
				}
			}

			return packageIndex;
		}
		
		void InitializeState(Core::PackageProvider& packageProvider, Graph& packagesGraph)
		{
			auto& packageGraph = packageProvider.GetRootPackageGraph();
			InitializeGraph(packageProvider, packagesGraph, packageGraph.RootPackageId);
			packagesGraph.Vertices = _state.PackagesIdList.size();

			_state.ShowAsciiArt = true;
			_state.ShowPackagesGraphView = 0;
			_state.ShowChildGraphView = 0;
			_state.PackagesListSelected = 0;
			_state.PackageTabSelected = 0;
		}

		TreeValueTable ToTreeValue(const Core::PackageChildrenMap& children)
		{
			auto dependencyProperties = TreeValueTable();
			for (auto& [dependencyType, dependencies] : children)
			{
				auto dependencyItems = TreeValueList();
				for (auto& dependency : dependencies)
				{
					dependencyItems.push_back(TreeValue(dependency.OriginalReference.ToString()));
				}

				dependencyProperties.Insert(dependencyType, TreeValue(std::move(dependencyItems)));
			}

			return dependencyProperties;
		}

		ftxui::Components CreateAllPackageTabs(
			Core::FileSystemState& fileSystemState,
			Core::PackageProvider& packageProvider)
		{
			auto rootPackageGraphId = packageProvider.GetRootPackageGraphId();

			_state.PackagesState.resize(_state.PackagesIdList.size());

			auto tabComponents = ftxui::Components();
			for (auto i = 0u; i < _state.PackagesIdList.size(); i++)
			{
				auto packageId = _state.PackagesIdList[i];
				auto& packageState = _state.PackagesState[i];

				auto& packageInfo = packageProvider.GetPackageInfo(packageId);
				auto packageLoadState = LoadPackage(
					fileSystemState, packageProvider, rootPackageGraphId, packageId);

				auto properties = TreeValueTable();

				properties.Insert("Id", TreeValue(std::to_string(packageInfo.Id)));
				properties.Insert("Name", TreeValue(packageInfo.Name.ToString()));
				properties.Insert("Root", TreeValue(packageInfo.PackageRoot.ToString()));

				auto dependencyProperties = ToTreeValue(packageInfo.Dependencies);
				properties.Insert("Dependencies", TreeValue(std::move(dependencyProperties)));

				auto recipeProperties = RecipeTreeConverter::ToTreeValue(packageInfo.Recipe->GetTable());
				properties.Insert("Recipe", TreeValue(std::move(recipeProperties)));

				auto propertiesList = ScrollFrame(TreeView(std::move(properties)));

				auto packageTabList = std::vector<std::string>({
					"Properties",
				});
				auto packageTabComponents = ftxui::Components({
					propertiesList,
				});

				auto hasPreprocessor = packageLoadState.GeneratePhase1Result.has_value() && 
					packageLoadState.GeneratePhase1Result.value().HasPreprocessor();

				if (packageLoadState.GeneratePhase1Info.has_value())
				{
					auto& generatePhase1Info = packageLoadState.GeneratePhase1Info.value();
					auto selected = hasPreprocessor ? &packageState.SelectedPreprocessorTask : &packageState.SelectedTask;

					auto tasksViewRenderer = LayoutGeneratePhaseTasks(generatePhase1Info, selected, &_state.ShowChildGraphView);

					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessor Tasks");
					}
					else
					{
						packageTabList.push_back("Tasks");
					}

					packageTabComponents.push_back(tasksViewRenderer);
				}

				if (packageLoadState.GeneratePhase1Result.has_value())
				{
					auto selected = hasPreprocessor ? &packageState.SelectedPreprocessor : &packageState.SelectedOperation;

					auto operationsView = LayoutOperations(packageLoadState.GeneratePhase1Result.value().GetGraph(), selected, &_state.ShowChildGraphView);

					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessors");
					}
					else
					{
						packageTabList.push_back("Operations");
					}

					packageTabComponents.push_back(operationsView);
				}

				if (packageLoadState.GeneratePhase2Info.has_value())
				{
					auto& generatePhase2Info = packageLoadState.GeneratePhase2Info.value();
					auto selected = &packageState.SelectedTask;

					auto tasksViewRenderer = LayoutGeneratePhaseTasks(generatePhase2Info, selected, &_state.ShowChildGraphView);

					packageTabList.push_back("Tasks");
					packageTabComponents.push_back(tasksViewRenderer);
				}

				if (packageLoadState.GeneratePhase2Result.has_value())
				{
					auto selected = &packageState.SelectedOperation;

					auto operationsView = LayoutOperations(packageLoadState.GeneratePhase2Result.value(), selected, &_state.ShowChildGraphView);

					packageTabList.push_back("Operations");
					packageTabComponents.push_back(operationsView);
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
