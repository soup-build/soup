﻿// <copyright file="OperationInfo.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "FileSystemState.h"
using namespace std::chrono_literals;

namespace Soup::Core
{
	export class CommandInfo
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="CommandInfo"/> class.
		/// </summary>
		CommandInfo() :
			WorkingDirectory(),
			Executable(),
			Arguments()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="CommandInfo"/> class.
		/// </summary>
		CommandInfo(
			Path workingDirectory,
			Path executable,
			std::string arguments) :
			WorkingDirectory(std::move(workingDirectory)),
			Executable(std::move(executable)),
			Arguments(std::move(arguments))
		{
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const CommandInfo& rhs) const
		{
			return WorkingDirectory == rhs.WorkingDirectory &&
				Executable == rhs.Executable &&
				Arguments == rhs.Arguments;
		}

		Path WorkingDirectory;
		Path Executable;
		std::string Arguments;
	};

	export using OperationId = uint32_t;

	export class OperationInfo
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="OperationInfo"/> class.
		/// </summary>
		OperationInfo() :
			Id(0),
			Title(),
			Command(),
			DeclaredInput(),
			DeclaredOutput(),
			ReadAccess(),
			WriteAccess(),
			Children(),
			DependencyCount(0),
			WasSuccessfulRun(false),
			EvaluateTime(std::chrono::time_point<std::chrono::system_clock>::min()),
			ObservedInput(),
			ObservedOutput()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="OperationInfo"/> class.
		/// </summary>
		OperationInfo(
			OperationId id,
			std::string title,
			CommandInfo command,
			std::vector<FileId> declaredInput,
			std::vector<FileId> declaredOutput,
			std::vector<FileId> readAccess,
			std::vector<FileId> writeAccess) :
			Id(id),
			Title(std::move(title)),
			Command(std::move(command)),
			DeclaredInput(std::move(declaredInput)),
			DeclaredOutput(std::move(declaredOutput)),
			ReadAccess(std::move(readAccess)),
			WriteAccess(std::move(writeAccess)),
			Children(),
			DependencyCount(0),
			WasSuccessfulRun(false),
			EvaluateTime(std::chrono::time_point<std::chrono::system_clock>::min()),
			ObservedInput(),
			ObservedOutput()
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="OperationInfo"/> class.
		/// </summary>
		OperationInfo(
			OperationId id,
			std::string title,
			CommandInfo command,
			std::vector<FileId> declaredInput,
			std::vector<FileId> declaredOutput,
			std::vector<FileId> readAccess,
			std::vector<FileId> writeAccess,
			std::vector<OperationId> children,
			uint32_t dependencyCount,
			bool wasSuccessfulRun,
			std::chrono::time_point<std::chrono::system_clock> evaluateTime,
			std::vector<FileId> observedInput,
			std::vector<FileId> observedOutput) :
			Id(id),
			Title(std::move(title)),
			Command(std::move(command)),
			DeclaredInput(std::move(declaredInput)),
			DeclaredOutput(std::move(declaredOutput)),
			ReadAccess(std::move(readAccess)),
			WriteAccess(std::move(writeAccess)),
			Children(std::move(children)),
			DependencyCount(dependencyCount),
			WasSuccessfulRun(wasSuccessfulRun),
			EvaluateTime(evaluateTime),
			ObservedInput(std::move(observedInput)),
			ObservedOutput(std::move(observedOutput))
		{
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const OperationInfo& rhs) const
		{
			return Id == rhs.Id &&
				Title == rhs.Title &&
				Command == rhs.Command &&
				DeclaredInput == rhs.DeclaredInput &&
				DeclaredOutput == rhs.DeclaredOutput &&
				ReadAccess == rhs.ReadAccess &&
				WriteAccess == rhs.WriteAccess &&
				Children == rhs.Children &&
				DependencyCount == rhs.DependencyCount &&
				WasSuccessfulRun == rhs.WasSuccessfulRun &&
				EvaluateTime == rhs.EvaluateTime &&
				ObservedInput == rhs.ObservedInput &&
				ObservedOutput == rhs.ObservedOutput;
		}

		OperationId Id;
		std::string Title;
		CommandInfo Command;
		std::vector<FileId> DeclaredInput;
		std::vector<FileId> DeclaredOutput;
		std::vector<FileId> ReadAccess;
		std::vector<FileId> WriteAccess;
		std::vector<OperationId> Children;
		uint32_t DependencyCount;
		bool WasSuccessfulRun;
		std::chrono::time_point<std::chrono::system_clock> EvaluateTime;
		std::vector<FileId> ObservedInput;
		std::vector<FileId> ObservedOutput;
	};
}

namespace std
{
	template<> struct hash<Soup::Core::CommandInfo>
	{
		std::size_t operator()(Soup::Core::CommandInfo const& value) const noexcept
		{
			std::size_t hashWorkingDirectory = std::hash<std::string>{}(value.WorkingDirectory.ToString());
			std::size_t hashExecutable = std::hash<std::string>{}(value.Executable.ToString());
			std::size_t hashArguments = std::hash<std::string>{}(value.Arguments);
			return hashWorkingDirectory ^ (hashExecutable << 1) ^ (hashArguments << 2);
		}
	};
}