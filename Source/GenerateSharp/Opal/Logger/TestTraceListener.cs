// <copyright file="TestTraceListener.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Opal;

/// <summary>
/// Test logger that wraps the base <see cref="TraceListener"/>.
/// </summary>
public class TestTraceListener : TraceListener
{
	private readonly List<string> _messages;

	/// <summary>
	/// Initializes a new instance of the <see cref='TestTraceListener'/> class.
	/// </summary>
	public TestTraceListener()
		: base(null, true, false)
	{
		this._messages = [];
	}

	/// <summary>
	/// Get the messages.
	/// </summary>
	public IReadOnlyList<string> Messages => this._messages;

	/// <summary>
	/// Writes a message and newline terminator.
	/// </summary>
	/// <param name="message">The message.</param>
	protected override void WriteLine(string message)
	{
		this._messages.Add(message);
	}
}
