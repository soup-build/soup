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

			WriteValue(stream, state.IsPreprocessor());

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
			WriteValues(stream, state.GetGraph().GetRootOperationIds());

			// Write out the set of operations
			auto& operations = state.GetGraph().GetOperations();
			stream.write("OPS\0", 4);
			WriteValue(stream, static_cast<uint32_t>(operations.size()));
			for (auto& operationValue : operations)
			{
				OperationGraphWriter::WriteOperationInfo(stream, operationValue.second);
			}
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