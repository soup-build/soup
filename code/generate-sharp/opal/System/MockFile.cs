﻿// <copyright file="MockFile.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.IO;

namespace Opal.System;

public class MockFile
{
	public MemoryStream Content { get; set; }

	public int LastWriteTime { get; set; }

	public MockFile(MemoryStream content, int lastWriteTime)
	{
		this.Content = content;
		this.LastWriteTime = lastWriteTime;
	}

	public MockFile(MemoryStream content)
	{
		this.Content = content;
		this.LastWriteTime = 0;
	}

	public MockFile(int lastWriteTime)
	{
		this.Content = new MemoryStream();
		this.LastWriteTime = lastWriteTime;
	}

	public MockFile()
	{
		this.Content = new MemoryStream();
		this.LastWriteTime = 0;
	}
}

public class MockOutputFile : IOutputFile
{
	private readonly MockFile file;
	private bool isDisposed;

	public MockOutputFile(MockFile file)
	{
		this.isDisposed = false;
		this.file = file;
	}

	/// <summary>
	/// Gets the file stream.
	/// </summary>
	public Stream GetOutStream()
	{
		return this.file.Content;
	}

	public void Dispose()
	{
		// Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
		Dispose(disposing: true);
		global::System.GC.SuppressFinalize(this);
	}

	protected virtual void Dispose(bool disposing)
	{
		if (!this.isDisposed)
		{
			this.isDisposed = true;
		}
	}
}

public class MockInputFile : IInputFile
{
	private readonly MockFile file;
	private bool isDisposed;

	public MockInputFile(MockFile file)
	{
		this.isDisposed = false;
		this.file = file;
	}

	/// <summary>
	/// Gets the file stream.
	/// </summary>
	public Stream GetInStream()
	{
		_ = this.file.Content.Seek(0, SeekOrigin.Begin);
		return this.file.Content;
	}

	public void Dispose()
	{
		// Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
		Dispose(disposing: true);
		global::System.GC.SuppressFinalize(this);
	}

	protected virtual void Dispose(bool disposing)
	{
		if (!this.isDisposed)
		{
			this.isDisposed = true;
		}
	}
}