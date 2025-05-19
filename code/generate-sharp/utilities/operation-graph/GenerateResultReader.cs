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

		// Read the set of operation proxies
		headerBuffer = reader.ReadBytes(4);
		if (headerBuffer[0] != 'O' ||
			headerBuffer[1] != 'P' ||
			headerBuffer[2] != 'P' ||
			headerBuffer[3] != '\0')
		{
			throw new InvalidOperationException("Invalid generate result operation proxies header");
		}

		var operationProxyCount = reader.ReadUInt32();
		var operationProxies = new List<OperationProxyInfo>();
		for (var i = 0; i < operationProxyCount; i++)
		{
			operationProxies.Add(ReadOperationProxyInfo(reader));
		}

		if (reader.BaseStream.Position != reader.BaseStream.Length)
		{
			var remaining = reader.BaseStream.Length - reader.BaseStream.Position;
			throw new InvalidOperationException($"Generate result file corrupted - Did not read the entire file {remaining}");
		}

		return new GenerateResult(
			files,
			new OperationGraph(
				files,
				rootOperationIds,
				operations),
			operationProxies);
	}

	private static OperationProxyInfo ReadOperationProxyInfo(System.IO.BinaryReader reader)
	{
		// Read the operation proxy id
		var id = new OperationProxyId(reader.ReadUInt32());

		// Read the operation proxy title
		var title = ReadString(reader);

		// Read the command working directory
		var workingDirectory = ReadString(reader);

		// Read the command executable
		var executable = ReadString(reader);

		// Read the command arguments
		var arguments = ReadStringList(reader);

		// Read the declared input files
		var declaredInput = ReadFileIdList(reader);

		// Read the result file
		var resultFile = new FileId(reader.ReadUInt32());

		// Read the command working directory
		var finalizerTask = ReadString(reader);

		// Read the finalizer state
		var finalizerState = ValueTableReader.ReadValueTable(reader);

		// Read the read access list
		var readAccess = ReadFileIdList(reader);

		return new OperationProxyInfo(
			id,
			title,
			new CommandInfo(
				new Path(workingDirectory),
				new Path(executable),
				arguments),
			declaredInput,
			resultFile,
			finalizerTask,
			finalizerState,
			readAccess);
	}

	private static string ReadString(System.IO.BinaryReader reader)
	{
		var size = reader.ReadUInt32();
		var result = reader.ReadChars((int)size);

		return new string(result);
	}

	private static List<string> ReadStringList(System.IO.BinaryReader reader)
	{
		var size = reader.ReadUInt32();
		var result = new List<string>((int)size);
		for (var i = 0; i < size; i++)
		{
			result.Add(ReadString(reader));
		}

		return result;
	}

	private static List<FileId> ReadFileIdList(System.IO.BinaryReader reader)
	{
		var size = reader.ReadUInt32();
		var result = new List<FileId>((int)size);
		for (var i = 0; i < size; i++)
		{
			result.Add(new FileId(reader.ReadUInt32()));
		}

		return result;
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
