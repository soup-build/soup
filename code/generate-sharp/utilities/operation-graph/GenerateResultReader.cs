// <copyright file="GenerateResultReader.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;
using Path = Opal.Path;

namespace Soup.Build.Utilities;

/// <summary>
/// The generate result state reader
/// </summary>
internal static class GenerateResultReader
{
	// Binary Generate Result file format
	private static uint FileVersion => 1;

	public static GenerateResult Deserialize(System.IO.BinaryReader reader)
	{
		// Read the File Header with version
		var headerBuffer = reader.ReadBytes(4);
		if (headerBuffer[0] != 'B' ||
			headerBuffer[1] != 'G' ||
			headerBuffer[2] != 'R' ||
			headerBuffer[3] != '\0')
		{
			throw new InvalidOperationException("Invalid generate result file header");
		}

		var fileVersion = reader.ReadUInt32();
		if (fileVersion != FileVersion)
		{
			throw new InvalidOperationException("Generate result file version does not match expected");
		}

		var isPreprocessor = ReadBoolean(reader);

		// Read the set of files
		headerBuffer = reader.ReadBytes(4);
		if (headerBuffer[0] != 'F' ||
			headerBuffer[1] != 'I' ||
			headerBuffer[2] != 'S' ||
			headerBuffer[3] != '\0')
		{
			throw new InvalidOperationException("Invalid generate result files header");
		}

		var fileCount = reader.ReadUInt32();
		var files = new List<(FileId FileId, Path Path)>();
		for (var i = 0; i < fileCount; i++)
		{
			// Read the command working directory
			var fileId = new FileId(reader.ReadUInt32());
			var file = new Path(ReadString(reader));

			files.Add((fileId, file));
		}

		// Read the set of operations
		headerBuffer = reader.ReadBytes(4);
		if (headerBuffer[0] != 'R' ||
			headerBuffer[1] != 'O' ||
			headerBuffer[2] != 'P' ||
			headerBuffer[3] != '\0')
		{
			throw new InvalidOperationException("Invalid generate result root operations header");
		}

		// Read the root operation ids
		var rootOperationIds = ReadOperationIdList(reader);

		// Read the set of operations
		headerBuffer = reader.ReadBytes(4);
		if (headerBuffer[0] != 'O' ||
			headerBuffer[1] != 'P' ||
			headerBuffer[2] != 'S' ||
			headerBuffer[3] != '\0')
		{
			throw new InvalidOperationException("Invalid generate result operations header");
		}

		var operationCount = reader.ReadUInt32();
		var operations = new List<OperationInfo>();
		for (var i = 0; i < operationCount; i++)
		{
			operations.Add(OperationGraphReader.ReadOperationInfo(reader));
		}

		if (reader.BaseStream.Position != reader.BaseStream.Length)
		{
			var remaining = reader.BaseStream.Length - reader.BaseStream.Position;
			throw new InvalidOperationException($"Generate result file corrupted - Did not read the entire file {remaining}");
		}

		return new GenerateResult(
			new OperationGraph(
				files,
				rootOperationIds,
				operations),
			isPreprocessor);
	}

	private static bool ReadBoolean(System.IO.BinaryReader reader)
	{
		uint result = reader.ReadUInt32();
		return result != 0;
	}

	private static string ReadString(System.IO.BinaryReader reader)
	{
		var size = reader.ReadUInt32();
		var result = reader.ReadChars((int)size);

		return new string(result);
	}

	private static List<OperationId> ReadOperationIdList(System.IO.BinaryReader reader)
	{
		var size = reader.ReadUInt32();
		var result = new List<OperationId>((int)size);
		for (var i = 0; i < size; i++)
		{
			result.Add(new OperationId(reader.ReadUInt32()));
		}

		return result;
	}
}
