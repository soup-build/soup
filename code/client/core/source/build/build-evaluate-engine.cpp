// <copyright file="build-evaluate-engine.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <algorithm>
#include <format>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

export module Soup.Core:BuildEvaluateEngine;

import :BuildEvaluateGraphState;
import :BuildFailedException;
import :BuildHistoryChecker;
import :FileSystemState;
import :IEvaluateEngine;
import :OperationInfo;
import :OperationGraph;
import :OperationResult;
import :OperationResults;
import :SystemAccessTracker;
import Monitor.Host;
import Opal;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The core build evaluation engine that knows how to perform a build from a provided Operation Graph.
	/// </summary>
	export class BuildEvaluateEngine : public IEvaluateEngine
	{
	private:
		unsigned int _threadCount;
		bool _forceRebuild;
		bool _disableMonitor;
		bool _partialMonitor;

		// Shared Runtime State
		FileSystemState& _fileSystemState;
		BuildHistoryChecker _stateChecker;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="BuildEvaluateEngine"/> class.
		/// </summary>
		BuildEvaluateEngine(
			unsigned int threadCount,
			bool forceRebuild,
			bool disableMonitor,
			bool partialMonitor,
			FileSystemState& fileSystemState) :
			_threadCount(threadCount),
			_forceRebuild(forceRebuild),
			_disableMonitor(disableMonitor),
			_partialMonitor(partialMonitor),
			_fileSystemState(fileSystemState),
			_stateChecker(fileSystemState)
		{
		}

		/// <summary>
		/// Execute the entire operation graph that is referenced by this build evaluate engine
		/// </summary>
		bool Evaluate(
			OperationGraph& operationGraph,
			OperationResults& operationResults,
			const Path& temporaryDirectory,
			const std::vector<Path>& globalAllowedReadAccess,
			const std::vector<Path>& globalAllowedWriteAccess,
			std::optional<std::function<ValueTable(std::string_view)>> processStdOut) override
		{
			const auto& rootOperations = operationGraph.GetRootOperationIds();

			if (!rootOperations.empty())
			{
				// Run all build operations in the correct order with incremental build checks
				Log::Diag("Build evaluation start {}", _threadCount);
				auto evaluateState = BuildEvaluateGraphState(
					operationGraph,
					operationResults,
					temporaryDirectory,
					globalAllowedReadAccess,
					globalAllowedWriteAccess,
					processStdOut);

				// Initialize the ready set from the root operations
				evaluateState.AddReadyOperations(rootOperations);

				auto workerThreads = std::vector<std::thread>();
				for (auto i = 1u; i <= _threadCount; i++)
				{
					workerThreads.push_back(
						std::thread(
							&BuildEvaluateEngine::WorkerThread,
							this,
							std::ref(evaluateState),
							i));
				}

				// Ensure all other workers have finished
				for (auto& worker : workerThreads)
				{
					worker.join();
				}

				Log::Diag("Build evaluation end");
				return evaluateState.DidAnyEvaluate();
			}
			else
			{
				Log::Diag("Build evaluation skipped");
				return false;
			}
		}

	private:
		void WorkerThread(BuildEvaluateGraphState& evaluateState, size_t id)
		{
			Log::Diag("Worker thread start {}", id);

			// Process all operations until non are available
			auto operationState = evaluateState.WaitNextOperation();
			while (operationState.has_value())
			{
				try
				{
					// Evaluate the current operation
					CheckExecuteOperation(evaluateState, operationState.value());

					// Scan the children for those ready to be built
					evaluateState.RegisterReadyChildren(operationState.value().Info);

					// Pop the next operation available or wait for one to be queued up
					operationState = evaluateState.WaitNextOperation();
				}
				catch (...)
				{
					evaluateState.SetError(std::current_exception());
					Log::Diag("Worker thread error {}", id);
					return;
				}
			}

			Log::Diag("Worker thread end {}", id);
		}

		/// <summary>
		/// Check if an individual operation has been run and execute if required
		/// </summary>
		void CheckExecuteOperation(
			BuildEvaluateGraphState& evaluateState,
			const CurrentOperationState operationState)
		{
			// Check if each source file is out of date and requires a rebuild
			Log::Diag("Check for previous operation invocation");

			// Check if this operation was run before
			auto buildRequired = false;
			if (operationState.PreviousResult &&
				operationState.PreviousResult->WasSuccessfulRun)
			{
				// Check if the executable has changed since the last run
				bool executableOutOfDate = false;
				if (operationState.Info.Command.Executable != Path("./writefile.exe"))
				{
					// Only check for "real" executables
					auto executableFileId = _fileSystemState.ToFileId(
						operationState.Info.Command.Executable,
						operationState.Info.Command.WorkingDirectory);
					if (_stateChecker.IsOutdated(operationState.PreviousResult->EvaluateTime, executableFileId))
					{
						Log::Diag("Executable out of date");
						executableOutOfDate = true;
					}
				}

				// Perform the incremental build checks
				if (executableOutOfDate ||
					_stateChecker.IsOutdated(
						operationState.PreviousResult->ObservedOutput,
						operationState.PreviousResult->ObservedInput))
				{
					buildRequired = true;
				}
				else
				{
					if (_forceRebuild)
					{
						Log::HighPriority("Up to date: Force Build");
						buildRequired = true;
					}
					else
					{
						Log::Info("Up to date");
					}
				}
			}
			else
			{
				// The build command has not been run before
				Log::Info("Operation has no successful previous invocation");
				buildRequired = true;
			}

			if (buildRequired)
			{
				Log::HighPriority(operationState.Info.Title);
				auto messageBuilder = std::stringstream();
				messageBuilder << "Execute: [" << operationState.Info.Command.WorkingDirectory.ToString() << "] ";
				messageBuilder << operationState.Info.Command.Executable.ToString();
				for (auto& argument : operationState.Info.Command.Arguments)
					messageBuilder << " " << argument;

				Log::Diag(messageBuilder.str());

				auto operationResult = OperationResult();

				// Check for special in-process write operations
				if (operationState.Info.Command.Executable == Path("./writefile.exe"))
				{
					ExecuteWriteFileOperation(
						operationState.Info,
						operationResult);
				}
				else
				{
					ExecuteOperation(
						evaluateState.TemporaryDirectory,
						evaluateState.GlobalAllowedReadAccess,
						evaluateState.GlobalAllowedWriteAccess,
						evaluateState.ProcessStdOut,
						operationState.Info,
						operationResult);
				}

				// Ensure there are no new dependencies
				VerifyObservedState(evaluateState, operationState.Info, operationResult);

				evaluateState.UpdateOperationResult(
					operationState.Info.Id,
					std::move(operationResult));
			}
			else
			{
				Log::Info(operationState.Info.Title);
			}
		}

		/// <summary>
		/// Execute a single build operation
		/// </summary>
		void ExecuteWriteFileOperation(
			const OperationInfo& operationInfo,
			OperationResult& operationResult)
		{
			Log::Info("Execute InProcess WriteFile");

			// Pull out the file path argument
			if (operationInfo.Command.Arguments.size() != 2)
			{
				Log::Error("WriteFile path argument malformed");
				throw BuildFailedException();
			}

			auto fileName = Path(operationInfo.Command.Arguments[0]);
			Log::Info("WritFile: {}", fileName.ToString());

			auto filePath = fileName.HasRoot() ? fileName : operationInfo.Command.WorkingDirectory + fileName;
			auto& content = operationInfo.Command.Arguments[1];

			// Open the file to write to
			auto file = System::IFileSystem::Current().OpenWrite(filePath, false);
			file->GetOutStream() << content;

			operationResult.ObservedInput = {};
			operationResult.ObservedOutput = {
				_fileSystemState.ToFileId(filePath, operationInfo.Command.WorkingDirectory),
			};

			// Mark this operation as successful to enable future incremental builds
			operationResult.WasSuccessfulRun = true;
			operationResult.EvaluateTime = System::ISystem::Current().GetCurrentTime();

			// Ensure the File System State is notified of any output files that have changed
			_fileSystemState.InvalidateFileWriteTimes(operationResult.ObservedOutput);
		}

		/// <summary>
		/// Execute a single build operation
		/// </summary>
		void ExecuteOperation(
			const Path& temporaryDirectory,
			const std::vector<Path>& globalAllowedReadAccess,
			const std::vector<Path>& globalAllowedWriteAccess,
			std::optional<std::function<ValueTable(std::string_view)>> processStdOut,
			const OperationInfo& operationInfo,
			OperationResult& operationResult)
		{
			auto monitor = std::make_shared<SystemAccessTracker>();

			// Add the temp folder to the environment
			auto environment = std::map<std::string, std::string>();
			environment.emplace("TEMP", temporaryDirectory.ToString());
			environment.emplace("TMP", temporaryDirectory.ToString());

			// Allow access to the declared inputs/outputs
			bool enableAccessChecks = true;
			auto allowedReadAccess = std::vector<Path>();
			auto allowedWriteAccess = std::vector<Path>();
			if (enableAccessChecks)
			{
				allowedReadAccess = _fileSystemState.GetFilePaths(operationInfo.ReadAccess);
				allowedWriteAccess = _fileSystemState.GetFilePaths(operationInfo.WriteAccess);
			}

			// Allow access to the global overrides
			std::copy(globalAllowedReadAccess.begin(), globalAllowedReadAccess.end(), std::back_inserter(allowedReadAccess));
			std::copy(globalAllowedWriteAccess.begin(), globalAllowedWriteAccess.end(), std::back_inserter(allowedWriteAccess));

			Log::Diag("Allowed Read Access:");
			for (auto& file : allowedReadAccess)
				Log::Diag(file.ToString());
			Log::Diag("Allowed Write Access:");
			for (auto& file : allowedWriteAccess)
				Log::Diag(file.ToString());

			std::shared_ptr<System::IProcess> process = nullptr;
			if (_disableMonitor)
			{
				process = System::IProcessManager::Current().CreateProcess(
					operationInfo.Command.Executable,
					operationInfo.Command.Arguments,
					operationInfo.Command.WorkingDirectory,
					true);
			}
			else
			{
				process = Monitor::IMonitorProcessManager::Current().CreateMonitorProcess(
					operationInfo.Command.Executable,
					operationInfo.Command.Arguments,
					operationInfo.Command.WorkingDirectory,
					environment,
					monitor,
					enableAccessChecks,
					_partialMonitor,
					std::move(allowedReadAccess),
					std::move(allowedWriteAccess));
			}

			process->Start();
			process->WaitForExit();

			auto stdOut = process->GetStandardOutput();
			auto stdErr = process->GetStandardError();
			auto exitCode = process->GetExitCode();

			// Check the result of the monitor
			monitor->VerifyResult();

			if (!stdOut.empty())
			{
				// Upgrade output to a warning if the command fails
				if (exitCode != 0)
					Log::Warning(stdOut);
				else
					Log::Diag(stdOut);
			}

			// If there was any error output then the build failed
			// TODO: Find warnings + errors
			if (!stdErr.empty())
			{
				Log::Error(stdErr);
			}

			if (exitCode == 0)
			{
				// Save off the build graph for future builds
				auto input = std::vector<Path>();
				for (auto& value : monitor->GetInput())
				{
					auto path = Path::Parse(value);
					#ifdef TRACE_FILE_SYSTEM_STATE
						Log::Diag("ObservedInput: {}", path.ToString());
					#endif
					input.push_back(std::move(path));
				}

				auto output = std::vector<Path>();
				for (auto& value : monitor->GetOutput())
				{
					auto path = Path::Parse(value);
					#ifdef TRACE_FILE_SYSTEM_STATE
						Log::Diag("ObservedOutput: {}", path.ToString());
					#endif
					output.push_back(std::move(path));
				}

				operationResult.ObservedInput = _fileSystemState.ToFileIds(
					input,
					operationInfo.Command.WorkingDirectory);
				operationResult.ObservedOutput = _fileSystemState.ToFileIds(
					output,
					operationInfo.Command.WorkingDirectory);

				// Mark this operation as successful to enable future incremental builds
				operationResult.WasSuccessfulRun = true;
				operationResult.EvaluateTime = System::ISystem::Current().GetCurrentTime();

				if (processStdOut.has_value())
				{
					operationResult.ObservedValues = processStdOut.value()(stdOut);
				}

				// Ensure the File System State is notified of any output files that have changed
				_fileSystemState.InvalidateFileWriteTimes(operationResult.ObservedOutput);
			}
			else
			{
				// Leave the previous state untouched and abandon the remaining operations
				Log::Error("Operation exited with non-success code: {}", exitCode);
				throw BuildFailedException();
			}
		}

		void VerifyObservedState(
			const BuildEvaluateGraphState& evaluateState,
			const OperationInfo& operationInfo,
			OperationResult& operationResult)
		{
			// TODO: Should generate NEW input/output lookup to check for entirely observed dependencies

			// Verify new inputs
			for (auto fileId : operationResult.ObservedInput)
			{
				// Check if this input was generated from another operation
				OperationId matchedOutputOperationId;
				if (evaluateState.TryGetOutputFileOperation(fileId, matchedOutputOperationId) &&
					operationInfo.Id != matchedOutputOperationId)
				{
					// If it is a known output file then it must be a declared input for this operation
					const std::set<OperationId>* matchedInputOperationIds;
					if (!evaluateState.TryGetInputFileOperations(fileId, matchedInputOperationIds) ||
						!matchedInputOperationIds->contains(operationInfo.Id))
					{
						auto filePath = _fileSystemState.GetFilePath(fileId);
						auto& existingOperation = evaluateState.Graph.GetOperationInfo(matchedOutputOperationId);
						auto message = std::format(
							"File \"{}\" observed as input for operation \"{}\" was written to by operation \"{}\" and must be declared as input",
							filePath.ToString(),
							operationInfo.Title,
							existingOperation.Title);
						throw std::runtime_error(message);
					}
				}
			}

			// Verify new outputs
			for (auto fileId : operationResult.ObservedOutput)
			{
				// Ensure the file is not also an output
				auto findObservedInput = std::find(operationResult.ObservedInput.begin(), operationResult.ObservedInput.end(), fileId);
				if (findObservedInput != operationResult.ObservedInput.end())
				{
					auto filePath = _fileSystemState.GetFilePath(fileId);
					Log::Warning("File \"{}\" observed as both input and output for operation \"{}\"", filePath.ToString(), operationInfo.Title);
					Log::Warning("Removing from input list for now. Will be treated as error in the future.");
					operationResult.ObservedInput.erase(findObservedInput);
				}

				// Ensure declared output is compatible
				OperationId matchedOutputOperationId;
				if (evaluateState.TryGetOutputFileOperation(fileId, matchedOutputOperationId))
				{
					if (matchedOutputOperationId != operationInfo.Id)
					{
						auto filePath = _fileSystemState.GetFilePath(fileId);
						auto& existingOperation = evaluateState.Graph.GetOperationInfo(matchedOutputOperationId);
						auto message = std::format(
							"File \"{}\" observed as output for operation \"{}\" was already written by operation \"{}\"",
							filePath.ToString(),
							operationInfo.Title,
							existingOperation.Title);
						throw std::runtime_error(message);
					}
				}
				else
				{
					// Ensure new ouput does not create a dependency connection
					const std::set<OperationId>* matchedInputOperationIds;
					if (evaluateState.TryGetInputFileOperations(fileId, matchedInputOperationIds))
					{
						if (!matchedInputOperationIds->contains(operationInfo.Id))
						{
							auto filePath = _fileSystemState.GetFilePath(fileId);
							auto message = std::format(
								"File \"{}\" observed as output from operation \"{}\" creates new dependency to existing declared inputs",
								filePath.ToString(),
								operationInfo.Title);
							throw std::runtime_error(message);
						}
					}
				}
			}
		}
	};
}