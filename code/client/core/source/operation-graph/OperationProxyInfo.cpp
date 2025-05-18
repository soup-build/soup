// <copyright file="OperationInfo.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <string>

export module Soup.Core:OperationProxyInfo;

import Opal;
import :CommandInfo;
import :FileSystemState;

using namespace Opal;
using namespace std::chrono_literals;

export namespace Soup::Core
{
	using OperationProxyId = uint32_t;

	/// <summary>
	/// A node in the build graph that is the smallest operation proxy that will generate real operations
	/// </summary>
	class OperationProxyInfo
	{
	public:
		OperationProxyId Id;
		std::string Title;
		CommandInfo Command;
		std::vector<FileId> DeclaredInput;
		FileId ResultFile;
		std::string FinalizerTask;

		std::vector<FileId> ReadAccess;

	public:
		OperationProxyInfo() :
			Id(0),
			Title(),
			Command(),
			DeclaredInput(),
			ResultFile(),
			FinalizerTask(),
			ReadAccess()
		{
		}

		OperationProxyInfo(
			OperationProxyId id,
			std::string title,
			CommandInfo command,
			std::vector<FileId> declaredInput,
			FileId resultFile,
			std::string finalizerTask,
			std::vector<FileId> readAccess) :
			Id(id),
			Title(std::move(title)),
			Command(std::move(command)),
			DeclaredInput(std::move(declaredInput)),
			ResultFile(resultFile),
			FinalizerTask(std::move(finalizerTask)),
			ReadAccess(std::move(readAccess))
		{
		}

		bool operator ==(const OperationProxyInfo& rhs) const
		{
			return Id == rhs.Id &&
				Title == rhs.Title &&
				Command == rhs.Command &&
				DeclaredInput == rhs.DeclaredInput &&
				ResultFile == rhs.ResultFile &&
				FinalizerTask == rhs.FinalizerTask &&
				ReadAccess == rhs.ReadAccess;
		}
	};
}