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
import :GraphLayout;
import :GraphView;
import :PackageLoadState;
import :RecipeTreeConverter;
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
			InitializeState(packageProvider);

			auto app = ftxui::App::Fullscreen();

			auto asciiArt = AppAsciiArt(&_state.ShowAsciiArt);

			// TEST create graph
			auto positions = LayoutDAG(_state.PackagesGraph);

			//auto packagesMenu = ScrollFrame(
			//	CreateSingleItemMenu(_state.PackagesList, &_state.PackagesListSelected));

			auto packagesMenu = ScrollFrame(GraphView(std::move(positions), _state.PackagesNameList));

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
		int InitializeGraph(
			Core::PackageProvider& packageProvider,
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
						auto childIndex = InitializeGraph(packageProvider, dependency.PackageId);

						// Add an edge for the graph
						_state.PackagesGraph.Edges.push_back({packageIndex, childIndex});
					}
				}
			}

			return packageIndex;
		}
		
		void InitializeState(Core::PackageProvider& packageProvider)
		{
			auto& packageGraph = packageProvider.GetRootPackageGraph();
			InitializeGraph(packageProvider, packageGraph.RootPackageId);
			_state.PackagesGraph.Vertices = _state.PackagesIdList.size();

			_state.ShowAsciiArt = true;
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
					auto findRuntimeOrderResult = generatePhase1Info.find("RuntimeOrder");
					if (findRuntimeOrderResult == generatePhase1Info.end())
					{
						throw std::runtime_error("Generate Info Table missing RuntimeOrder List");
					}

					auto findTaskInfoTableResult = generatePhase1Info.find("TaskInfo");
					if (findTaskInfoTableResult == generatePhase1Info.end())
					{
						throw std::runtime_error("Generate Info Table missing TaskInfo List");
					}
					auto& taskInfoTable = findTaskInfoTableResult->second.AsTable();

					auto tasksComponents = std::vector<std::string>();
					auto tasksPropertiesComponents = ftxui::Components();
					for (auto& taskNameValue : findRuntimeOrderResult->second.AsList())
					{
						auto taskName = taskNameValue.AsString();
						tasksComponents.push_back(taskName);

						auto findTaskInfoResult = taskInfoTable.find(taskName);
						if (findTaskInfoResult == taskInfoTable.end())
						{
							throw std::runtime_error(std::format("TaskInfo missing task {}", taskName));
						}

						auto taskInfo = ValueTreeConverter::ToTreeValue(findTaskInfoResult->second.AsTable());
						tasksPropertiesComponents.push_back(
							ScrollFrame(TreeView(std::move(taskInfo))));
					}

					auto selected = hasPreprocessor ? &packageState.SelectedPreprocessorTask : &packageState.SelectedTask;

					auto tasksList = ScrollFrame(
						CreateSingleItemMenu(std::move(tasksComponents), selected));

					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessor Tasks");
					}
					else
					{
						packageTabList.push_back("Tasks");
					}

					auto tasksPropertiesView = ftxui::Container::Tab(
						std::move(tasksPropertiesComponents),
						selected);

					auto tasksView = ftxui::Container::Horizontal({
						tasksList,
						tasksPropertiesView,
					});

					auto tasksViewRenderer = ftxui::Renderer(tasksView, [tasksList, tasksPropertiesView] {
						return ftxui::hbox({
							tasksList->Render(),
							ftxui::separator(),
							tasksPropertiesView->Render() | ftxui::xflex,
						}) | ftxui::yflex;
					});

					packageTabComponents.push_back(tasksViewRenderer);
				}

				if (packageLoadState.GeneratePhase1Result.has_value())
				{
					auto operationComponents = std::vector<std::string>();
					auto operationPropertiesComponents = ftxui::Components();
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase1Result.value().GetGraph().GetOperations())
					{
						operationComponents.push_back(operation.Title);

						auto operationInfo = TreeValueTable();

						operationInfo.Insert("Id", TreeValue(std::to_string(operation.Id)));
						operationInfo.Insert("Title", TreeValue(operation.Title));

						auto commandInfo = TreeValueTable();
						commandInfo.Insert("WorkingDirectory", operation.Command.WorkingDirectory.ToString());
						commandInfo.Insert("Executable", operation.Command.Executable.ToString());
						
						auto arguments = TreeValueList();
						for (auto& argument : operation.Command.Arguments)
						{
							arguments.push_back(TreeValue(argument));
						}

						commandInfo.Insert("Arguments", std::move(arguments));
						operationInfo.Insert("Command", TreeValue(std::move(commandInfo)));

						// std::vector<FileId> DeclaredInput;
						// std::vector<FileId> DeclaredOutput;

						operationPropertiesComponents.push_back(
							ScrollFrame(TreeView(std::move(operationInfo))));
					}

					auto selected = hasPreprocessor ? &packageState.SelectedPreprocessor : &packageState.SelectedOperation;

					auto operationsList = ScrollFrame(
						CreateSingleItemMenu(std::move(operationComponents), selected));

					if (hasPreprocessor)
					{
						packageTabList.push_back("Preprocessors");
					}
					else
					{
						packageTabList.push_back("Operations");
					}

					auto operationsPropertiesView = ftxui::Container::Tab(
						std::move(operationPropertiesComponents),
						selected);

					auto operationsView = ftxui::Container::Horizontal({
						operationsList,
						operationsPropertiesView,
					});

					auto operationsViewRenderer = ftxui::Renderer(operationsView, [operationsList, operationsPropertiesView] {
						return ftxui::hbox({
							operationsList->Render(),
							ftxui::separator(),
							operationsPropertiesView->Render() | ftxui::xflex,
						}) | ftxui::yflex;
					});

					packageTabComponents.push_back(operationsViewRenderer);
				}

				if (packageLoadState.GeneratePhase2Info.has_value())
				{
					auto& generatePhase2Info = packageLoadState.GeneratePhase2Info.value();
					auto findRuntimeOrderResult = generatePhase2Info.find("RuntimeOrder");
					if (findRuntimeOrderResult == generatePhase2Info.end())
					{
						throw std::runtime_error("Generate Info Table missing RuntimeOrder List");
					}

					auto findTaskInfoTableResult = generatePhase2Info.find("TaskInfo");
					if (findTaskInfoTableResult == generatePhase2Info.end())
					{
						throw std::runtime_error("Generate Info Table missing TaskInfo List");
					}
					auto& taskInfoTable = findTaskInfoTableResult->second.AsTable();

					auto tasksComponents = std::vector<std::string>();
					auto tasksPropertiesComponents = ftxui::Components();
					for (auto& taskNameValue : findRuntimeOrderResult->second.AsList())
					{
						auto taskName = taskNameValue.AsString();
						tasksComponents.push_back(taskName);

						auto findTaskInfoResult = taskInfoTable.find(taskName);
						if (findTaskInfoResult == taskInfoTable.end())
						{
							throw std::runtime_error(std::format("TaskInfo missing task {}", taskName));
						}

						auto taskInfo = ValueTreeConverter::ToTreeValue(findTaskInfoResult->second.AsTable());
						tasksPropertiesComponents.push_back(
							ScrollFrame(TreeView(std::move(taskInfo))));
					}

					auto selected = &packageState.SelectedTask;

					auto tasksList = ScrollFrame(
						CreateSingleItemMenu(std::move(tasksComponents), selected));

					packageTabList.push_back("Tasks");

					auto tasksPropertiesView = ftxui::Container::Tab(
						std::move(tasksPropertiesComponents),
						selected);

					auto tasksView = ftxui::Container::Horizontal({
						tasksList,
						tasksPropertiesView,
					});

					auto tasksViewRenderer = ftxui::Renderer(tasksView, [tasksList, tasksPropertiesView] {
						return ftxui::hbox({
							tasksList->Render(),
							ftxui::separator(),
							tasksPropertiesView->Render() | ftxui::xflex,
						}) | ftxui::yflex;
					});

					packageTabComponents.push_back(tasksViewRenderer);
				}

				if (packageLoadState.GeneratePhase2Result.has_value())
				{
					auto operationComponents = std::vector<std::string>();
					auto operationPropertiesComponents = ftxui::Components();
					for (auto& [operationId, operation] : packageLoadState.GeneratePhase2Result.value().GetOperations())
					{
						operationComponents.push_back(operation.Title);

						auto operationInfo = TreeValueTable();

						operationInfo.Insert("Id", TreeValue(std::to_string(operation.Id)));
						operationInfo.Insert("Title", TreeValue(operation.Title));

						auto commandInfo = TreeValueTable();
						commandInfo.Insert("WorkingDirectory", operation.Command.WorkingDirectory.ToString());
						commandInfo.Insert("Executable", operation.Command.Executable.ToString());
						
						auto arguments = TreeValueList();
						for (auto& argument : operation.Command.Arguments)
						{
							arguments.push_back(TreeValue(argument));
						}

						commandInfo.Insert("Arguments", std::move(arguments));
						operationInfo.Insert("Command", TreeValue(std::move(commandInfo)));

						// std::vector<FileId> DeclaredInput;
						// std::vector<FileId> DeclaredOutput;

						operationPropertiesComponents.push_back(
							ScrollFrame(TreeView(std::move(operationInfo))));
					}

					auto selected = &packageState.SelectedOperation;

					auto operationsList = ScrollFrame(
						CreateSingleItemMenu(std::move(operationComponents), selected));

					packageTabList.push_back("Operations");

					auto operationsPropertiesView = ftxui::Container::Tab(
						std::move(operationPropertiesComponents),
						selected);

					auto operationsView = ftxui::Container::Horizontal({
						operationsList,
						operationsPropertiesView,
					});

					auto operationsViewRenderer = ftxui::Renderer(operationsView, [operationsList, operationsPropertiesView] {
						return ftxui::hbox({
							operationsList->Render(),
							ftxui::separator(),
							operationsPropertiesView->Render() | ftxui::xflex,
						}) | ftxui::yflex;
					});

					packageTabComponents.push_back(operationsViewRenderer);
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
