﻿// <copyright file="OperationGraphWriter.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <cstdint>
#include <cstring>
#include <iostream>
#include <set>
#include <vector>

export module Soup.Core:OperationGraphWriter;

import Opal;
import :FileSystemState;
import :OperationGraph;
import :OperationInfo;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The operation graph state writer
	/// </summary>
	export class OperationGraphWriter
	{
	private:
		// Binary Operation graph file format
		static constexpr uint32_t FileVersion = 6;

	public:
		static void Serialize(
			const OperationGraph& state,
			const std::set<FileId>& files,
			const FileSystemState& fileSystemState,
			std::ostream& stream)
		{
			// Write the File Header with version
			stream.write("BOG\0", 4);
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
			WriteValues(stream, state.GetRootOperationIds());

			// Write out the set of operations
			auto& operations = state.GetOperations();
			stream.write("OPS\0", 4);
			WriteValue(stream, static_cast<uint32_t>(operations.size()));
			for (auto& operationValue : operations)
			{
				WriteOperationInfo(stream, operationValue.second);
			}
		}

		static void WriteOperationInfo(std::ostream& stream, const OperationInfo& operation)
		{
			// Write out the operation id
			WriteValue(stream, operation.Id);

			// Write out the operation title
			WriteValue(stream, operation.Title);

			// Write the command working directory
			WriteValue(stream, operation.Command.WorkingDirectory.ToString());

			// Write the command executable
			WriteValue(stream, operation.Command.Executable.ToString());

			// Write the command arguments
			WriteValues(stream, operation.Command.Arguments);

			// Write out the declared input files
			WriteValues(stream, operation.DeclaredInput);

			// Write out the declared output files
			WriteValues(stream, operation.DeclaredOutput);

			// Write out the read access list
			WriteValues(stream, operation.ReadAccess);

			// Write out the write access list
			WriteValues(stream, operation.WriteAccess);

			// Write out the child operation ids
			WriteValues(stream, operation.Children);

			// Write out the dependency count
			WriteValue(stream, operation.DependencyCount);
		}

	private:
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