// <copyright file="GenerateResultManager.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <memory>
#include <set>
#include <stdexcept>

export module Soup.Core:GenerateResultManager;

import Opal;
import :FileSystemState;
import :GenerateResult;
import :GenerateResultReader;
import :GenerateResultWriter;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The generate result state manager
	/// </summary>
	export class GenerateResultManager
	{
	public:
		/// <summary>
		/// Load the generate result state from the provided directory
		/// </summary>
		static bool TryLoadState(
			const Path& generateResultFile,
			GenerateResult& result,
			FileSystemState& fileSystemState)
		{
			// Open the file to read from
			std::shared_ptr<System::IInputFile> file;
			if (!System::IFileSystem::Current().TryOpenRead(generateResultFile, true, file))
			{
				Log::Info("Generate result file does not exist");
				return false;
			}

			// Read the contents of the build state file
			try
			{
				result = GenerateResultReader::Deserialize(file->GetInStream(), fileSystemState);
				return true;
			}
			catch(std::runtime_error& ex)
			{
				Log::Error(ex.what());
				return false;
			}
			catch(...)
			{
				Log::Error("Failed to parse generate result");
				return false;
			}
		}

		/// <summary>
		/// Save the generate result state for the provided directory
		/// </summary>
		static void SaveState(
			const Path& generateResultFile,
			GenerateResult& state,
			const FileSystemState& fileSystemState)
		{
			auto targetFolder = generateResultFile.GetParent();

			// Update the generate result referenced files
			auto files = std::set<FileId>();
			for (auto& operationReference : state.GetEvaluateGraph().GetOperations())
			{
				auto& operation = operationReference.second;
				files.insert(operation.DeclaredInput.begin(), operation.DeclaredInput.end());
				files.insert(operation.DeclaredOutput.begin(), operation.DeclaredOutput.end());
				files.insert(operation.ReadAccess.begin(), operation.ReadAccess.end());
				files.insert(operation.WriteAccess.begin(), operation.WriteAccess.end());
			}

			for (auto& operationProxyReference : state.GetOperationProxies())
			{
				auto& operationProxy = operationProxyReference.second;
				files.insert(operationProxy.DeclaredInput.begin(), operationProxy.DeclaredInput.end());
				files.insert(operationProxy.ReadAccess.begin(), operationProxy.ReadAccess.end());
			}

			// Open the file to write to
			auto file = System::IFileSystem::Current().OpenWrite(generateResultFile, true);

			// Write the build state to the file stream
			GenerateResultWriter::Serialize(state, files, fileSystemState, file->GetOutStream());
		}
	};
}