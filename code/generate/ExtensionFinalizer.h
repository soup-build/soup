// <copyright file="ExtensionFinalizer.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "ExtensionTaskDetails.h"
#include "GenerateHost.h"

namespace Soup::Core::Generate
{
	/// <summary>
	/// The extension Finalizer
	/// </summary>
	class ExtensionFinalizer
	{
	private:
		std::map<std::string, ExtensionTaskDetails> _tasks;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ExtensionFinalizer"/> class.
		/// </summary>
		ExtensionFinalizer() :
			_tasks()
		{
		}

		/// <summary>
		/// Register extension task
		/// </summary>
		void RegisterExtensionTask(ExtensionTaskDetails extensionTaskDetails)
		{
			auto name = extensionTaskDetails.Name;
			Log::Diag("RegisterExtensionTask: {}", name);

			auto insertResult = _tasks.emplace(name, std::move(extensionTaskDetails));
			if (!insertResult.second)
			{
				Log::HighPriority("An extension task with the provided name has already been registered: {}", name);
				throw std::runtime_error("An extension task with the provided name has already been registered");
			}
		}

		/// <summary>
		/// Execute all build extensions
		/// </summary>
		void Execute(GenerateState& state)
		{
			auto runtimeOrderList = ValueList();
			auto extensionTaskInfoTable = ValueTable();

			// Run all finalizer tasks
			for (auto& [taskName, currentTask] : _tasks)
			{
				// Create a Wren Host to evaluate the extension task
				auto host = std::make_unique<GenerateHost>(currentTask.ScriptFile, currentTask.BundlesFile);
				host->InterpretMain();

				// Set the current state AFTER we initialize to prevent pre-loading
				host->SetState(state);

				Log::Info("FinalizerTaskStart: {}", currentTask.Name);

				host->EvaluateFinalizerTask(currentTask.Name, "TEST RESULT");

				Log::Info("FinalizerTaskDone: {}", currentTask.Name);

				// Get the final state to be passed to the next extension
				auto updatedActiveState = host->GetUpdatedActiveState();
				auto updatedSharedState = host->GetUpdatedSharedState();

				auto runBeforeList = ValueList();
				for (const auto& value : currentTask.RunBeforeList)
					runBeforeList.push_back(Value(value));

				auto runAfterList = ValueList();
				for (const auto& value : currentTask.RunAfterList)
					runAfterList.push_back(Value(value));

				auto runAfterClosureList = ValueList();
				for (const auto& value : currentTask.RunAfterClosureList)
					runAfterClosureList.push_back(Value(value));

				// Build the extension task info
				auto extensionTaskInfo = ValueTable();
				extensionTaskInfo.emplace("ActiveState", Value(updatedActiveState));
				extensionTaskInfo.emplace("SharedState", Value(updatedSharedState));
				extensionTaskInfo.emplace("RunBeforeList", Value(std::move(runBeforeList)));
				extensionTaskInfo.emplace("RunAfterList", Value(std::move(runAfterList)));
				extensionTaskInfo.emplace("RunAfterClosureList", Value(std::move(runAfterClosureList)));

				extensionTaskInfoTable.emplace(currentTask.Name, Value(std::move(extensionTaskInfo)));
				runtimeOrderList.push_back(Value(currentTask.Name));

				// Mark the extension task completed
				currentTask.HasRun = true;

				// Update state for next extension task
				Log::Info("UpdateState");
				state.Update(std::move(updatedActiveState), std::move(updatedSharedState));
			}

			// Store the runtime information for easy debugging
			auto generateInfoTable = ValueTable();
			generateInfoTable.emplace("Version", Value("0.1"));
			generateInfoTable.emplace("RuntimeOrder", Value(std::move(runtimeOrderList)));
			generateInfoTable.emplace("TaskInfo", Value(std::move(extensionTaskInfoTable)));
			generateInfoTable.emplace("GlobalState", Value(state.GetGlobalState()));

			state.SetGenerateInfo(std::move(generateInfoTable));
		}
	};
}