﻿// <copyright file="ExtensionManager.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "ExtensionTaskDetails.h"
#include "GenerateHost.h"

namespace Soup::Core::Generate
{
	/// <summary>
	/// The extension manager
	/// </summary>
	class ExtensionManager
	{
	private:
		std::map<std::string, ExtensionTaskDetails> _preprocessorTasks;
		std::map<std::string, ExtensionTaskDetails> _extensionTasks;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ExtensionManager"/> class.
		/// </summary>
		ExtensionManager() :
			_preprocessorTasks(),
			_extensionTasks()
		{
		}

		/// <summary>
		/// Register preprocessor task
		/// </summary>
		void RegisterPreprocessorTask(ExtensionTaskDetails preprocessorTaskDetails)
		{
			auto name = preprocessorTaskDetails.Name;
			Log::Diag("RegisterPreprocessorTask: {}", name);

			auto insertResult = _preprocessorTasks.emplace(name, std::move(preprocessorTaskDetails));
			if (!insertResult.second)
			{
				Log::HighPriority("An preprocessor task with the provided name has already been registered: {}", name);
				throw std::runtime_error("An preprocessor task with the provided name has already been registered");
			}
		}

		/// <summary>
		/// Register extension task
		/// </summary>
		void RegisterExtensionTask(ExtensionTaskDetails extensionTaskDetails)
		{
			auto name = extensionTaskDetails.Name;
			Log::Diag("RegisterExtensionTask: {}", name);

			auto runBeforeMessage = std::stringstream();
			runBeforeMessage << "RunBefore [";
			bool isFirst = true;
			for (auto& value : extensionTaskDetails.RunBeforeList)
			{
				if (!isFirst)
					runBeforeMessage << ", ";

				runBeforeMessage << "\"" << value << "\"";
				isFirst = false;
			}

			runBeforeMessage << "]";
			Log::Diag(runBeforeMessage.str());

			auto runAfterMessage = std::stringstream();
			runAfterMessage << "RunAfter [";
			isFirst = true;
			for (auto& value : extensionTaskDetails.RunAfterList)
			{
				if (!isFirst)
					runAfterMessage << ", ";

				runAfterMessage << "\"" << value << "\"";
				isFirst = false;
			}

			runAfterMessage << "]";
			Log::Diag(runAfterMessage.str());

			auto insertResult = _extensionTasks.emplace(name, std::move(extensionTaskDetails));
			if (!insertResult.second)
			{
				Log::HighPriority("An extension task with the provided name has already been registered: {}", name);
				throw std::runtime_error("An extension task with the provided name has already been registered");
			}
		}

		/// <summary>
		/// Get a value indicating if there are proprocessor tasks
		/// </summary>
		bool HasPreprocessorTasks()
		{
			return _preprocessorTasks.size() > 0;
		}

		/// <summary>
		/// Execute all build extensions
		/// </summary>
		void ExecutePreprocessorTasks(GenerateState& state)
		{
			Execute(state, _preprocessorTasks);
		}

		/// <summary>
		/// Execute all build extensions
		/// </summary>
		void ExecuteExtensionTasks(GenerateState& state)
		{
			Execute(state, _extensionTasks);
		}

	private:
		/// <summary>
		/// Execute all build extensions
		/// </summary>
		void Execute(GenerateState& state, std::map<std::string, ExtensionTaskDetails>& tasks)
		{
			// Setup each extension to have a complete list of extensions that must run before itself
			// Note: this is required to combine other extensions run before lists with the extensions
			// own run after list
			for (auto& [key, task] : tasks)
			{
				// Copy their own run after list
				task.RunAfterClosureList.insert(
					task.RunAfterClosureList.end(),
					task.RunAfterList.begin(),
					task.RunAfterList.end());

				// Add ourself to all tasks in our run before list
				for (auto& runBefore : task.RunBeforeList)
				{
					// Try to find the other task
					auto beforeTaskContainer = tasks.find(runBefore);
					if (beforeTaskContainer != tasks.end())
					{
						beforeTaskContainer->second.RunAfterClosureList.push_back(task.Name);
					}
				}
			}

			auto runtimeOrderList = ValueList();
			auto extensionTaskInfoTable = ValueTable();

			// Run all tasks in the order they were registered
			// ensuring they are run in the correct dependency order
			ExtensionTaskDetails* currentTask;
			while (TryFindNextTask(currentTask, tasks))
			{
				if (currentTask == nullptr)
					throw std::runtime_error("TryFindNextTask returned empty result");

				// Create a Wren Host to evaluate the extension task
				auto host = std::make_unique<GenerateHost>(currentTask->ScriptFile, currentTask->BundlesFile);
				host->InterpretMain();

				// Set the current state AFTER we initialize to prevent pre-loading
				host->SetState(state);

				Log::Info("TaskStart: {}", currentTask->Name);

				host->EvaluateTask(currentTask->Name);

				Log::Info("TaskDone: {}", currentTask->Name);

				// Get the final state to be passed to the next extension
				auto updatedActiveState = host->GetUpdatedActiveState();
				auto updatedSharedState = host->GetUpdatedSharedState();

				auto runBeforeList = ValueList();
				for (const auto& value : currentTask->RunBeforeList)
					runBeforeList.push_back(Value(value));

				auto runAfterList = ValueList();
				for (const auto& value : currentTask->RunAfterList)
					runAfterList.push_back(Value(value));

				auto runAfterClosureList = ValueList();
				for (const auto& value : currentTask->RunAfterClosureList)
					runAfterClosureList.push_back(Value(value));

				// Build the extension task info
				auto extensionTaskInfo = ValueTable();
				extensionTaskInfo.emplace("ActiveState", Value(updatedActiveState));
				extensionTaskInfo.emplace("SharedState", Value(updatedSharedState));
				extensionTaskInfo.emplace("RunBeforeList", Value(std::move(runBeforeList)));
				extensionTaskInfo.emplace("RunAfterList", Value(std::move(runAfterList)));
				extensionTaskInfo.emplace("RunAfterClosureList", Value(std::move(runAfterClosureList)));

				extensionTaskInfoTable.emplace(currentTask->Name, Value(std::move(extensionTaskInfo)));
				runtimeOrderList.push_back(Value(currentTask->Name));

				// Mark the extension task completed
				currentTask->HasRun = true;

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

		/// <summary>
		/// Try to find the next task that has yet to be run and is ready
		/// Returns false if all tasks have been run
		/// Throws error if we hit a deadlock
		/// </summary>
		bool TryFindNextTask(ExtensionTaskDetails*& extensionTask, std::map<std::string, ExtensionTaskDetails>& tasks)
		{
			// Find the next task that is ready to be run
			bool hasAnyStillWaiting = false;
			for (auto& [key, task] : tasks)
			{
				// Check if this extension has run already,
				// if not check if all if all upstream extensions have finished
				if (!task.HasRun)
				{
					hasAnyStillWaiting = true;

					// Check if all of their run after dependencies have already finished
					bool hasDependencyPending = false;
					for (auto& runBefore : task.RunAfterClosureList)
					{
						auto findResult = tasks.find(runBefore);
						if (findResult != tasks.end() && !findResult->second.HasRun)
						{
							// Found a dependency that hasn't run, keep trying
							hasDependencyPending = true;
							break;
						}
					}

					// All dependencies have finished
					// Let's run this one
					if (!hasDependencyPending)
					{
						extensionTask = &task;
						return true;
					}
				}
			}

			if (hasAnyStillWaiting)
				throw std::runtime_error("Hit deadlock in build extension dependencies.");

			extensionTask = nullptr;
			return false;
		}
	};
}