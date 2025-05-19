// <copyright file="OperationProxyInfo.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;
using System.Linq;

namespace Soup.Build.Utilities;

public record OperationProxyId(uint Value);

public class OperationProxyInfo : IEquatable<OperationProxyInfo>
{
	/// <summary>
	/// Initializes a new instance of the <see cref="OperationProxyInfo"/> class.
	/// </summary>
	public OperationProxyInfo() :
		this(
			new OperationProxyId(0),
			string.Empty,
			new CommandInfo(),
			[],
			new FileId(0),
			string.Empty,
			[],
			[])
	{
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="OperationProxyInfo"/> class.
	/// </summary>
	public OperationProxyInfo(
		OperationProxyId id,
		string title,
		CommandInfo command,
		IList<FileId> declaredInput,
		FileId resultFile,
		string finalizerTask,
		ValueTable finalizerState,
		IList<FileId> readAccess)
	{
		this.Id = id;
		this.Title = title;
		this.Command = command;
		this.DeclaredInput = declaredInput;
		this.ResultFile = resultFile;
		this.FinalizerTask = finalizerTask;
		this.FinalizerState = finalizerState;
		this.ReadAccess = readAccess;
	}

	public bool Equals(OperationProxyInfo? other)
	{
		if (other is null)
			return false;

		var result = this.Id == other.Id &&
			this.Title == other.Title &&
			this.Command == other.Command &&
			Enumerable.SequenceEqual(this.DeclaredInput, other.DeclaredInput) &&
			this.ResultFile == other.ResultFile &&
			this.FinalizerTask == other.FinalizerTask &&
			this.FinalizerState == other.FinalizerState &&
			Enumerable.SequenceEqual(this.ReadAccess, other.ReadAccess);

		return result;
	}

	public override bool Equals(object? obj)
	{
		return Equals(obj as OperationProxyInfo);
	}

	public override int GetHashCode()
	{
		return (this.Id.GetHashCode() * 0x100000) +
			(this.Title.GetHashCode(StringComparison.InvariantCulture) * 0x1000) +
			this.Command.GetHashCode();
	}

	public static bool operator ==(OperationProxyInfo lhs, OperationProxyInfo other)
	{
		if (lhs is null)
			return other is null;
		return lhs.Equals(other);
	}

	public static bool operator !=(OperationProxyInfo lhs, OperationProxyInfo other)
	{
		return !(lhs == other);
	}

	public OperationProxyId Id { get; init; }
	public string Title { get; init; }
	public CommandInfo Command { get; init; }
	public IList<FileId> DeclaredInput { get; init; }
	public FileId ResultFile { get; set; }
	public string FinalizerTask { get; init; }
	public ValueTable FinalizerState { get; init; }
	public IList<FileId> ReadAccess { get; init; }
}