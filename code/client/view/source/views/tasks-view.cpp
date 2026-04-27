// <copyright file="tasks-view.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <stdexcept>
#include <vector>

export module Soup.View:TasksView;

import ftxui;
import Soup.Core;
import :TreeView;
import :ValueTreeConverter;

namespace Soup::View
{
	ftxui::Component LayoutGeneratePhaseTasks(const Core::ValueTable& generatePhase1Info, int* selected)
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

		auto tasksList = ScrollFrame(
			CreateSingleItemMenu(std::move(tasksComponents), selected));

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

		return tasksViewRenderer;
	}
}
