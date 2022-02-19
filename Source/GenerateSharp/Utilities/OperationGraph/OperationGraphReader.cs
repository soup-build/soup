﻿// <copyright file="OperationGraphReader.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Soup.Build.Runtime;
using System;
using System.Collections.Generic;

namespace Soup.Build.Utilities
{
	/// <summary>
	/// The operation graph state reader
	/// </summary>
	internal static class OperationGraphReader
	{
		// Binary Operation Graph file format
		private static uint FileVersion => 4;

		public static OperationGraph Deserialize(System.IO.BinaryReader reader)
		{
			// Read the File Header with version
			var headerBuffer = reader.ReadBytes(4);
			if (headerBuffer[0] != 'B' ||
				headerBuffer[1] != 'O' ||
				headerBuffer[2] != 'G' ||
				headerBuffer[3] != '\0')
			{
				throw new InvalidOperationException("Invalid operation graph file header");
			}

			var fileVersion = reader.ReadUInt32();
			if (fileVersion != FileVersion)
			{
				throw new InvalidOperationException("Operation graph file version does not match expected");
			}

			// Read the set of files
			headerBuffer = reader.ReadBytes(4);
			if (headerBuffer[0] != 'F' ||
				headerBuffer[1] != 'I' ||
				headerBuffer[2] != 'S' ||
				headerBuffer[3] != '\0')
			{
				throw new InvalidOperationException("Invalid operation graph files header");
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
				throw new InvalidOperationException("Invalid operation graph root operations header");
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
				throw new InvalidOperationException("Invalid operation graph operations header");
			}

			var operationCount = reader.ReadUInt32();
			var operations = new List<OperationInfo>();
			for (var i = 0; i < operationCount; i++)
			{
				operations.Add(ReadOperationInfo(reader));
			}

			if (reader.BaseStream.Position != reader.BaseStream.Length)
			{
				var remaining = reader.BaseStream.Length - reader.BaseStream.Position;
				throw new InvalidOperationException($"Operation graph file corrupted - Did not read the entire file {remaining}");
			}

			return new OperationGraph(
				files,
				rootOperationIds,
				operations);
		}

		private static OperationInfo ReadOperationInfo(System.IO.BinaryReader reader)
		{
			// Read the operation id
			var id = new OperationId(reader.ReadUInt32());

			// Read the operation title
			var title = ReadString(reader);

			// Read the command working directory
			var workingDirectory = ReadString(reader);

			// Read the command executable
			var executable = ReadString(reader);

			// Read the command arguments
			var arguments = ReadString(reader);

			// Read the declared input files
			var declaredInput = ReadFileIdList(reader);

			// Read the declared output files
			var declaredOutput = ReadFileIdList(reader);

			// Read the read access list
			var readAccess = ReadFileIdList(reader);

			// Read the write access list
			var writeAccess = ReadFileIdList(reader);

			// Read the child operation ids
			var children = ReadOperationIdList(reader);

			// Read the dependency count
			var dependecyCount = reader.ReadUInt32();

			// Read the value indicating if there was a successful run
			var wasSuccessfulRun = ReadBoolean(reader);

			// Read the utc tick since January 1, 0001 at 00:00:00.000 in the Gregorian calendar
			var evaluateTime = new DateTime(reader.ReadInt64(), DateTimeKind.Utc);

			// Read the observed input files
			var observedInput = ReadFileIdList(reader);

			// Read the observed output files
			var observedOutput = ReadFileIdList(reader);

			return new OperationInfo(
				id,
				title,
				new CommandInfo(
					new Path(workingDirectory),
					new Path(executable),
					arguments),
				declaredInput,
				declaredOutput,
				readAccess,
				writeAccess,
				children,
				dependecyCount,
				wasSuccessfulRun,
				evaluateTime,
				observedInput,
				observedOutput);
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
}
