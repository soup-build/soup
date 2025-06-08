// <copyright file="GenerateResultReader.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>

export module Soup.Core:GenerateResultReader;

import Opal;
import :CommandInfo;
import :FileSystemState;
import :GenerateResult;
import :OperationGraph;
import :OperationGraphReader;
import :OperationInfo;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The generate result reader
	/// </summary>
	export class GenerateResultReader
	{
	private:
		// Binary generate result file format
		static constexpr uint32_t FileVersion = 1;

	public:
		static GenerateResult Deserialize(std::istream& stream, FileSystemState& fileSystemState)
		{
			// Read the entire file for fastest read operation
			stream.seekg(0, std::ios_base::end);
			auto size = stream.tellg();
			stream.seekg(0, std::ios_base::beg);

			auto contentBuffer = std::vector<char>(size);
			stream.read(contentBuffer.data(), size);
			auto data = contentBuffer.data();
			size_t offset = 0;
			
			auto result = Deserialize(data, size, offset, fileSystemState);

			if (offset != contentBuffer.size())
			{
				throw std::runtime_error("Generate Result file corrupted - Did not read the entire file");
			}

			return result;
		}

	private:
		static GenerateResult Deserialize(
			char* data, size_t size, size_t& offset, FileSystemState& fileSystemState)
		{
			// BUG: Why does this need to be at the start of the file?
			auto activeFileIdMap = std::unordered_map<FileId, FileId>();

			// Read the File Header with version
			auto headerBuffer = std::array<char, 4>();
			Read(data, size, offset, headerBuffer.data(), 4);
			if (headerBuffer[0] != 'B' ||
				headerBuffer[1] != 'G' ||
				headerBuffer[2] != 'R' ||
				headerBuffer[3] != '\0')
			{
				throw std::runtime_error("Invalid generate result file header");
			}

			auto fileVersion = ReadUInt32(data, size, offset);
			if (fileVersion != FileVersion)
			{
				throw std::runtime_error("Generate Result file version does not match expected");
			}

			auto isPreprocessor = ReadBoolean(data, size, offset);

			// Read the set of files
			Read(data, size, offset, headerBuffer.data(), 4);
			if (headerBuffer[0] != 'F' ||
				headerBuffer[1] != 'I' ||
				headerBuffer[2] != 'S' ||
				headerBuffer[3] != '\0')
			{
				throw std::runtime_error("Invalid generate result files header");
			}

			// Map up the incoming file ids to the active file system state ids
			auto fileCount = ReadUInt32(data, size, offset);
			for (auto i = 0u; i < fileCount; i++)
			{
				// Read the command working directory
				auto fileId = ReadUInt32(data, size, offset);

				auto fileString = ReadString(data, size, offset);
				auto file = Path(std::move(fileString));

				auto activeFileId = fileSystemState.ToFileId(file);
				auto insertResult = activeFileIdMap.emplace(fileId, activeFileId);
				if (!insertResult.second)
					throw std::runtime_error("Failed to insert file id lookup");
			}

			// Read the set of operations
			Read(data, size, offset, headerBuffer.data(), 4);
			if (headerBuffer[0] != 'R' ||
				headerBuffer[1] != 'O' ||
				headerBuffer[2] != 'P' ||
				headerBuffer[3] != '\0')
			{
				throw std::runtime_error("Invalid generate result root operations header");
			}

			// Read the root operation ids
			auto rootOperationIds = ReadOperationIdList(data, size, offset);

			// Read the set of operations
			Read(data, size, offset, headerBuffer.data(), 4);
			if (headerBuffer[0] != 'O' ||
				headerBuffer[1] != 'P' ||
				headerBuffer[2] != 'S' ||
				headerBuffer[3] != '\0')
			{
				throw std::runtime_error("Invalid generate result operations header");
			}

			auto operationCount = ReadUInt32(data, size, offset);
			auto operations = std::vector<OperationInfo>(operationCount);
			for (auto i = 0u; i < operationCount; i++)
			{
				operations[i] = OperationGraphReader::ReadOperationInfo(data, size, offset, activeFileIdMap);
			}

			return GenerateResult(
				OperationGraph(
					std::move(rootOperationIds),
					std::move(operations)),
				isPreprocessor);
		}

		static uint32_t ReadUInt32(char* data, size_t size, size_t& offset)
		{
			uint32_t result = 0;
			Read(data, size, offset, reinterpret_cast<char*>(&result), sizeof(uint32_t));

			return result;
		}

		static bool ReadBoolean(char* data, size_t size, size_t& offset)
		{
			uint32_t result = 0;
			Read(data, size, offset, reinterpret_cast<char*>(&result), sizeof(uint32_t));

			return result != 0;
		}

		static std::string ReadString(char* data, size_t size, size_t& offset)
		{
			auto stringLength = ReadUInt32(data, size, offset);
			auto result = std::string(stringLength, '\0');
			Read(data, size, offset, result.data(), stringLength);

			return result;
		}

		static std::vector<std::string> ReadStringList(char* data, size_t size, size_t& offset)
		{
			auto listSize = ReadUInt32(data, size, offset);
			auto result = std::vector<std::string>(listSize);
			for (auto i = 0u; i < listSize; i++)
			{
				result[i] = ReadString(data, size, offset);
			}

			return result;
		}

		static std::vector<FileId> ReadFileIdList(
			char* data, size_t size, size_t& offset, const std::unordered_map<FileId, FileId>& activeFileIdMap)
		{
			auto listSize = ReadUInt32(data, size, offset);
			auto result = std::vector<FileId>(listSize);
			for (auto i = 0u; i < listSize; i++)
			{
				auto fileId = ReadUInt32(data, size, offset);

				// Find the active file id that maps to the cached file id
				auto findActiveFileId = activeFileIdMap.find(fileId);
				if (findActiveFileId == activeFileIdMap.end())
					throw std::runtime_error("Could not find file id in active map");

				result[i] = findActiveFileId->second;
			}

			return result;
		}

		static std::vector<OperationId> ReadOperationIdList(char* data, size_t size, size_t& offset)
		{
			auto listSize = ReadUInt32(data, size, offset);
			auto result = std::vector<OperationId>(listSize);
			for (auto i = 0u; i < listSize; i++)
			{
				result[i] = ReadUInt32(data, size, offset);
			}

			return result;
		}

		static void Read(char* data, size_t size, size_t& offset, char* buffer, size_t count)
		{
			if (offset + count > size)
				throw new std::runtime_error("Tried to read past end of data");
			memcpy(buffer, data + offset, count);
			offset += count;
		}
	};
}