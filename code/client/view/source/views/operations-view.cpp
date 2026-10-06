// <copyright file="operations-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module Soup.View:OperationsView;

import ftxui;
import Soup.Core;
import :GraphValue;
import :GraphView;
import :TreeView;
import :ValueTreeConverter;

namespace Soup::View {
	std::string format_bool(bool value) {
		return value ? "true" : "false";
	}

	std::string format_time(std::chrono::time_point<std::chrono::file_clock> time) {
		auto sys_time =
			std::chrono::clock_cast<std::chrono::system_clock>(time);
		std::chrono::zoned_time local_time{std::chrono::current_zone(), sys_time};
		return std::format("{:%Y-%m-%d %H:%M:%S}", local_time);
	}

	ftxui::Component LayoutOperations(
		const Core::OperationGraph &graph,
		std::optional<Core::OperationResults>& operationResults,
		int *selected, int *showGraphView) {
		// Build up the id lookups
		auto operationLookup = std::unordered_map<int, int>();
		auto operationComponents = std::vector<std::string>();
		for (auto &[operationId, operation] : graph.GetOperations()) {
			operationLookup.emplace(operationId, static_cast<int>(operationComponents.size()));
			operationComponents.push_back(operation.Title);
		}

		auto operationPropertiesComponents = ftxui::Components();
		Graph operationsGraph = {};
		for (auto &[operationId, operation] : graph.GetOperations()) {
			auto operationIndex = operationLookup[operationId];
			operationLookup.emplace(operationId, static_cast<int>(operationComponents.size()));

			// Add edges for children
			for (auto childId : operation.Children) {
				auto childIndex = operationLookup[childId];
				operationsGraph.Edges.push_back({operationIndex, childIndex});
			}

			auto operationInfo = TreeValueTable();

			operationInfo.Insert("Id", TreeValue(std::to_string(operation.Id)));
			operationInfo.Insert("Title", TreeValue(operation.Title));

			auto resultInfo = TreeValueTable();
			if (operationResults.has_value()) {
				Core::OperationResult *operationResult;
				if (operationResults->TryFindResult(operation.Id, operationResult)) {
					resultInfo.Insert(
						"WasSuccessfulRun", TreeValue(format_bool(operationResult->WasSuccessfulRun)));
					resultInfo.Insert(
						"EvaluateTime", TreeValue(format_time(operationResult->EvaluateTime)));

					// std::vector<FileId> ObservedInput;
					// std::vector<FileId> ObservedOutput;
				}
			}

			operationInfo.Insert("Result", TreeValue(std::move(resultInfo)));

			auto commandInfo = TreeValueTable();
			commandInfo.Insert("WorkingDirectory", operation.Command.WorkingDirectory.ToString());
			commandInfo.Insert("Executable", operation.Command.Executable.ToString());

			auto arguments = TreeValueList();
			for (auto &argument : operation.Command.Arguments) {
				arguments.push_back(TreeValue(argument));
			}

			commandInfo.Insert("Arguments", std::move(arguments));
			operationInfo.Insert("Command", TreeValue(std::move(commandInfo)));

			// std::vector<FileId> DeclaredInput;
			// std::vector<FileId> DeclaredOutput;

			operationPropertiesComponents.push_back(
				ScrollFrame(TreeView(std::move(operationInfo))));
		}

		operationsGraph.Vertices = static_cast<int>(operationComponents.size());

		auto operationsMenu = ScrollFrame(CreateSingleItemMenu(operationComponents, selected));

		auto operationsPropertiesView =
			ftxui::Container::Tab(std::move(operationPropertiesComponents), selected);

		auto operationsView = ftxui::Container::Horizontal(
			{
				operationsMenu,
				operationsPropertiesView,
			});

		auto operationsViewRenderer =
			ftxui::Renderer(operationsView, [operationsMenu, operationsPropertiesView] {
				return ftxui::hbox(
						   {
							   operationsMenu->Render(),
							   ftxui::separator(),
							   operationsPropertiesView->Render() | ftxui::xflex,
						   }) |
					   ftxui::yflex;
			});

		auto operationsGraphView =
			ScrollFrame(GraphView(std::move(operationsGraph), operationComponents));

		auto operationsToggle = ftxui::Container::Tab(
			{
				std::move(operationsViewRenderer),
				std::move(operationsGraphView),
			},
			showGraphView);

		return operationsToggle;
	}
}
