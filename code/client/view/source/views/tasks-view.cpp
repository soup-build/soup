// <copyright file="tasks-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <stdexcept>
#include <unordered_map>
#include <vector>

export module Soup.View:TasksView;

import ftxui;
import Soup.Core;
import :GraphValue;
import :GraphView;
import :TreeView;
import :ValueTreeConverter;

namespace Soup::View
{
	ftxui::Component LayoutGeneratePhaseTasks(
		const Core::ValueTable& generatePhase1Info,
		int* selected,
		int* showGraphView)
	{
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

		// Build up the id lookups
		auto taskLookup = std::unordered_map<std::string, int>();
		auto tasksComponents = std::vector<std::string>();
		auto tasksPropertiesComponents = ftxui::Components();
		for (auto& taskNameValue : findRuntimeOrderResult->second.AsList())
		{
			auto index = tasksComponents.size();

			auto taskName = taskNameValue.AsString();
			taskLookup.emplace(taskName, index);
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

		// Build up the children edges
		Graph tasksGraph = {};
		for (auto& [taskName, taskInfoValue] : taskInfoTable)
		{
			auto& taskInfo = taskInfoValue.AsTable();
			auto taskIndex = taskLookup[taskName];

			// Build up the children set
			auto findRunAfterClosureResult = taskInfo.find("RunAfterClosureList");
			if (findRunAfterClosureResult == taskInfo.end())
			{
				throw std::runtime_error("TaskInfo missing RunAfterClosureList");
			}

			for (auto& parentValue : findRunAfterClosureResult->second.AsList())
			{
				auto& parentName = parentValue.AsString();
				auto parentIndex = taskLookup[parentName];

				tasksGraph.Edges.push_back({ parentIndex, taskIndex });
			}
		}

		auto tasksMenu = ScrollFrame(CreateSingleItemMenu(tasksComponents, selected));

		auto tasksPropertiesView = ftxui::Container::Tab(
			std::move(tasksPropertiesComponents),
			selected);

		auto tasksView = ftxui::Container::Horizontal({
			tasksMenu,
			tasksPropertiesView,
		});

		auto tasksViewRenderer = ftxui::Renderer(tasksView, [tasksMenu, tasksPropertiesView] {
			return ftxui::hbox({
				tasksMenu->Render(),
				ftxui::separator(),
				tasksPropertiesView->Render() | ftxui::xflex,
			}) | ftxui::yflex;
		});

		tasksGraph.Vertices = tasksComponents.size();
		auto tasksGraphView = ScrollFrame(GraphView(std::move(tasksGraph), tasksComponents));

		auto tasksToggle = ftxui::Container::Tab({
				std::move(tasksViewRenderer),
				std::move(tasksGraphView),
			},
			showGraphView);

		return tasksToggle;
	}
}
