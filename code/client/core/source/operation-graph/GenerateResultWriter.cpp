// <copyright file="GenerateResultWriter.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <vector>

export module Soup.Core:GenerateResultWriter;

import Opal;
import :FileSystemState;
import :GenerateResult;
import :OperationGraphWriter;
import :OperationProxyInfo;
import :ValueTableWriter;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The generate result writer
	/// </summary>
	export class GenerateResultWriter
	{
	private:
		// Binary generate result file format
		static constexpr uint32_t FileVersion = 1;

	public:
		static void Serialize(
			const GenerateResult& state,
			const std::set<FileId>& files,
			const FileSystemState& fileSystemState,
			std::ostream& stream)
		{
			// Write the File Header with version
			stream.write("BGR\0", 4);
			WriteValue(stream, FileVersion);

			// Write out the set of files
			stream.write("FIS\0", 4);
			WriteValue(stream, static_cast<uint32_t>(files.size()));
			for (auto fileId : files)
			{
				// Write the file id + path length + path
				WriteValue(stream, fileId);
				WriteValue(stream, fileSystemState.GetFilePath(fileId).ToString());
			}

			// Write out the root operation ids
			stream.write("ROP\0", 4);
			WriteValues(stream, state.GetEvaluateGraph().GetRootOperationIds());

			// Write out the set of operations
			auto& operations = state.GetEvaluateGraph().GetOperations();
			stream.write("OPS\0", 4);
			WriteValue(stream, static_cast<uint32_t>(operations.size()));
			for (auto& operationValue : operations)
			{
				OperationGraphWriter::WriteOperationInfo(stream, operationValue.second);
			}
			
			// Write out the set of operation proxies
			auto& operationProxies = state.GetOperationProxies();
			stream.write("OPP", 4);
			WriteValue(stream, static_cast<uint32_t>(operationProxies.size()));
			for (auto& operationProxyValue : operationProxies)
			{
				WriteOperationProxyInfo(stream, operationProxyValue.second);
			}
		}

	private:
		static void WriteOperationProxyInfo(
			std::ostream& stream,
			const OperationProxyInfo& operationProxy)
		{
			// Write out the operation proxy id
			WriteValue(stream, operationProxy.Id);

			// Write out the operation proxy title
			WriteValue(stream, operationProxy.Title);

			// Write the command working directory
			WriteValue(stream, operationProxy.Command.WorkingDirectory.ToString());

			// Write the command executable
			WriteValue(stream, operationProxy.Command.Executable.ToString());

			// Write the command arguments
			WriteValues(stream, operationProxy.Command.Arguments);

			// Write out the declared input files
			WriteValues(stream, operationProxy.DeclaredInput);

			// Write out the result file
			WriteValue(stream, operationProxy.ResultFile);

			// Write out the finalizer task
			WriteValue(stream, operationProxy.FinalizerTask);

			// Write out the finalizer task
			ValueTableWriter::WriteValueTable(stream, operationProxy.FinalizerState);

			// Write out the read access list
			WriteValues(stream, operationProxy.ReadAccess);
		}

		static void WriteValue(std::ostream& stream, uint32_t value)
		{
			stream.write(reinterpret_cast<char*>(&value), sizeof(uint32_t));
		}

		static void WriteValue(std::ostream& stream, int64_t value)
		{
			stream.write(reinterpret_cast<char*>(&value), sizeof(int64_t));
		}

		static void WriteValue(std::ostream& stream, bool value)
		{
			uint32_t integerValue = value ? 1u : 0u;
			stream.write(reinterpret_cast<char*>(&integerValue), sizeof(uint32_t));
		}

		static void WriteValue(std::ostream& stream, std::string_view value)
		{
			WriteValue(stream, static_cast<uint32_t>(value.size()));
			stream.write(value.data(), value.size());
		}

		static void WriteValues(std::ostream& stream, const std::vector<std::string>& values)
		{
			WriteValue(stream, static_cast<uint32_t>(values.size()));
			for (auto& value : values)
			{
				WriteValue(stream, value);
			}
		}

		static void WriteValues(std::ostream& stream, const std::vector<uint32_t>& values)
		{
			WriteValue(stream, static_cast<uint32_t>(values.size()));
			for (auto& value : values)
			{
				WriteValue(stream, value);
			}
		}
	};
}