// <copyright file="command-info.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;
using Path = Opal.Path;

namespace Soup.Build.Utilities;

public class CommandInfo : IEquatable<CommandInfo>
{
	/// <summary>
	/// Initializes a new instance of the <see cref="CommandInfo"/> class.
	/// </summary>
	public CommandInfo() :
		this(new Path(), new Path(), [])
	{
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="CommandInfo"/> class.
	/// </summary>
	public CommandInfo(
		Path workingDirectory,
		Path executable,
		IReadOnlyList<string> arguments)
	{
		this.WorkingDirectory = workingDirectory;
		this.Executable = executable;
		this.Arguments = arguments;
	}

	/// <summary>
	/// Equality operator
	/// </summary>
	public bool Equals(CommandInfo? other)
	{
		if (other is null)
			return false;
		return this.WorkingDirectory == other.WorkingDirectory &&
			this.Executable == other.Executable &&
			this.Arguments == other.Arguments;
	}

	public override bool Equals(object? obj)
	{
		return Equals(obj as CommandInfo);
	}

	public override int GetHashCode()
	{
		return (this.WorkingDirectory.GetHashCode() * 0x100000) + (this.Executable.GetHashCode() * 0x1000) + this.Arguments.GetHashCode();
	}

	public static bool operator ==(CommandInfo lhs, CommandInfo other)
	{
		if (lhs is null)
			return other is null;
		return lhs.Equals(other);
	}

	public static bool operator !=(CommandInfo lhs, CommandInfo other)
	{
		return !(lhs == other);
	}

	public Path WorkingDirectory { get; init; }
	public Path Executable { get; init; }
	public IReadOnlyList<string> Arguments { get; init; }
}