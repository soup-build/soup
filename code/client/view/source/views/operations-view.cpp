// <copyright file="operations-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <stdexcept>
#include <unordered_map>
#include <vector>

export module Soup.View:OperationsView;

import ftxui;
import Soup.Core;
import :GraphValue;
import :GraphView;
import :TreeView;
import :ValueTreeConverter;

namespace Soup::View
{
	ftxui::Component LayoutOperations(const Core::OperationGraph& graph, int* selected, int* showGraphView)
	{
		// Build up the id lookups
		auto operationLookup = std::unordered_map<int, int>();
		auto operationComponents = std::vector<std::string>();
		for (auto& [operationId, operation] : graph.GetOperations())
		{
			operationLookup.emplace(operationId, operationComponents.size());
			operationComponents.push_back(operation.Title);
		}

		auto operationPropertiesComponents = ftxui::Components();
		Graph operationsGraph = {};
		for (auto& [operationId, operation] : graph.GetOperations())
		{
			auto operationIndex = operationLookup[operationId];
			operationLookup.emplace(operationId, operationComponents.size());

			// Add edges for children
			for (auto childId : operation.Children)
			{
				auto childIndex = operationLookup[childId];
				operationsGraph.Edges.push_back({ operationIndex, childIndex });
			}

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

		operationsGraph.Vertices = operationComponents.size();

		auto operationsMenu = ScrollFrame(CreateSingleItemMenu(operationComponents, selected));

		auto operationsPropertiesView = ftxui::Container::Tab(
			std::move(operationPropertiesComponents),
			selected);

		auto operationsView = ftxui::Container::Horizontal({
			operationsMenu,
			operationsPropertiesView,
		});

		auto operationsViewRenderer = ftxui::Renderer(operationsView, [operationsMenu, operationsPropertiesView] {
			return ftxui::hbox({
				operationsMenu->Render(),
				ftxui::separator(),
				operationsPropertiesView->Render() | ftxui::xflex,
			}) | ftxui::yflex;
		});

		auto operationsGraphView = ScrollFrame(GraphView(std::move(operationsGraph), operationComponents));

		auto operationsToggle = ftxui::Container::Tab({
				std::move(operationsViewRenderer),
				std::move(operationsGraphView),
			},
			showGraphView);

		return operationsToggle;
	}
}
