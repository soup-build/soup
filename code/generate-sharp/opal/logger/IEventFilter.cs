// <copyright file="IEventFilter.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;

namespace Opal;

[Flags]
public enum TraceEvents
{
	/// <summary>
	/// None.
	/// </summary>
	None = 0,

	/// <summary>
	/// High Priority message.
	/// </summary>
	HighPriority = 1 << 0,

	/// <summary>
	/// Informational message.
	/// </summary>
	Information = 1 << 1,

	/// <summary>
	/// Diagnostic trace.
	/// </summary>
	Diagnostic = 1 << 2,

	/// <summary>
	/// Noncritical problem.
	/// </summary>
	Warning = 1 << 3,

	/// <summary>
	/// Recoverable error.
	/// </summary>
	Error = 1 << 4,

	/// <summary>
	/// Fatal error or application crash.
	/// </summary>
	Critical = 1 << 5,
}

public interface IEventFilter
{
	bool ShouldTrace(TraceEvents eventType);
}
