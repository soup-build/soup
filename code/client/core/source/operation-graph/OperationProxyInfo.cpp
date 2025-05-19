// <copyright file="OperationProxyInfo.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <map>
#include <string>

export module Soup.Core:OperationProxyInfo;

import Opal;
import :CommandInfo;
import :FileSystemState;
import :Value;

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
		ValueTable FinalizerState;

		std::vector<FileId> ReadAccess;

	public:
		OperationProxyInfo() :
			Id(0),
			Title(),
			Command(),
			DeclaredInput(),
			ResultFile(),
			FinalizerTask(),
			FinalizerState(),
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
			ValueTable finalizerState,
			std::vector<FileId> readAccess) :
			Id(id),
			Title(std::move(title)),
			Command(std::move(command)),
			DeclaredInput(std::move(declaredInput)),
			ResultFile(resultFile),
			FinalizerTask(std::move(finalizerTask)),
			FinalizerState(std::move(finalizerState)),
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
				FinalizerState == rhs.FinalizerState &&
				ReadAccess == rhs.ReadAccess;
		}
	};
}