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
		std::map<std::string, ExtensionTaskDetails> _finalizerTasks;
		ValueTable _extensionTaskInfoTable;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ExtensionFinalizer"/> class.
		/// </summary>
		ExtensionFinalizer() :
			_finalizerTasks(),
			_extensionTaskInfoTable()
		{
		}

		/// <summary>
		/// Register extension task
		/// </summary>
		void RegisterFinalizerTask(ExtensionTaskDetails extensionTaskDetails)
		{
			auto name = extensionTaskDetails.Name;
			Log::Diag("RegisterFinalizerTask: {}", name);

			auto insertResult = _finalizerTasks.emplace(name, std::move(extensionTaskDetails));
			if (!insertResult.second)
			{
				Log::HighPriority("An extension task with the provided name has already been registered: {}", name);
				throw std::runtime_error("An extension task with the provided name has already been registered");
			}
		}

		/// <summary>
		/// Execute all build extensions
		/// </summary>
		void Execute(GenerateState& state, const OperationProxyInfo& operationProxy)
		{
			// Run all finalizer tasks
			auto finalizerTaskResult = _finalizerTasks.find(operationProxy.FinalizerTask);
			if (finalizerTaskResult == _finalizerTasks.end())
			{
				Log::HighPriority("Finalizer Task does not exist: {}", operationProxy.FinalizerTask);
				throw std::runtime_error("Missing finalizer task");
			}

			auto& currentTask = finalizerTaskResult->second;

			// Create a Wren Host to evaluate the extension task
			auto host = std::make_unique<GenerateHost>(currentTask.ScriptFile, currentTask.BundlesFile);
			host->InterpretMain();

			// Set the current state AFTER we initialize to prevent pre-loading
			host->SetState(state);

			Log::Info("FinalizerTaskStart: {}", currentTask.Name);

			auto finalizerResult = std::string("TEST RESULT");

			host->EvaluateFinalizerTask(currentTask.Name, operationProxy.FinalizerState, finalizerResult);

			Log::Info("FinalizerTaskDone: {}", currentTask.Name);

			// Build the extension task info
			auto extensionTaskInfo = ValueTable();
			extensionTaskInfo.emplace("FinalizerState", Value(operationProxy.FinalizerState));
			extensionTaskInfo.emplace("FinalizerResult", Value(finalizerResult));

			_extensionTaskInfoTable.emplace(currentTask.Name, Value(std::move(extensionTaskInfo)));
		}

		void BuildGenerateInfo(GenerateState& state)
		{
			// Store the runtime information for easy debugging
			auto generateInfoTable = ValueTable();
			generateInfoTable.emplace("Version", Value(std::string("0.1")));
			generateInfoTable.emplace("TaskInfo", Value(std::move(_extensionTaskInfoTable)));

			state.SetGenerateInfo(std::move(generateInfoTable));
		}
	};
}