﻿// <copyright file="ValueTableManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using System;
using System.Text;
using Path = Opal.Path;

namespace Soup.Build.Utilities;

/// <summary>
/// The Value Table state manager
/// </summary>
public static class ValueTableManager
{
	/// <summary>
	/// Load the value table from the target file
	/// </summary>
	public static bool TryLoadState(
		Path valueTableFile,
		out ValueTable result)
	{
		// Verify the requested file exists
		if (!LifetimeManager.Get<IFileSystem>().Exists(valueTableFile))
		{
			Log.Info("Value Table file does not exist");
			result = [];
			return false;
		}

		// Open the file to read from
		using var file = LifetimeManager.Get<IFileSystem>().OpenRead(valueTableFile);
		using var reader = new System.IO.BinaryReader(file.GetInStream(), Encoding.UTF8, true);

		// Read the contents of the build state file
		try
		{
			result = ValueTableReader.Deserialize(reader);
			return true;
		}
		catch (InvalidOperationException ex)
		{
			Log.Error($"Failed to parse value table: {ex.Message}");
			result = [];
			return false;
		}
	}

	/// <summary>
	/// Save the value table for the target file
	/// </summary>
	public static void SaveState(
		Path valueTableFile,
		ValueTable state)
	{
		// Open the file to write to
		using var fileStream = System.IO.File.Open(valueTableFile.ToString(), System.IO.FileMode.Create, System.IO.FileAccess.Write);
		using var writer = new System.IO.BinaryWriter(fileStream);

		// Write the build state to the file stream
		ValueTableWriter.Serialize(state, writer);
	}
}