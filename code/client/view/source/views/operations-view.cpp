// <copyright file="operations-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <stdexcept>
#include <vector>

export module Soup.View:OperationsView;

import ftxui;
import Soup.Core;
import :TreeView;
import :ValueTreeConverter;

namespace Soup::View
{
	ftxui::Component LayoutOperations(const Core::OperationGraph& graph, int* selected)
	{
		auto operationComponents = std::vector<std::string>();
		auto operationPropertiesComponents = ftxui::Components();
		for (auto& [operationId, operation] : graph.GetOperations())
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

		auto operationsList = ScrollFrame(
			CreateSingleItemMenu(std::move(operationComponents), selected));

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
		
		return operationsViewRenderer;
	}
}
