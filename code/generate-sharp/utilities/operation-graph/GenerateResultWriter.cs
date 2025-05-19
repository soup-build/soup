// <copyright file="GenerateResultWriter.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;
using System.IO;

namespace Soup.Build.Utilities;

/// <summary>
/// The generate result state writer
/// </summary>
internal static class GenerateResultWriter
{
	// Binary Generate Result file format
	private static uint FileVersion => 1;

	internal static readonly char[] FIS = ['F', 'I', 'S', '\0'];
	internal static readonly char[] BGR = ['B', 'G', 'R', '\0'];
	internal static readonly char[] ROP = ['R', 'O', 'P', '\0'];
	internal static readonly char[] OPS = ['O', 'P', 'S', '\0'];
	internal static readonly char[] OPP = ['O', 'P', 'P', '\0'];

	public static void Serialize(GenerateResult state, BinaryWriter writer)
	{
		// Write the File Header with version
		writer.Write(BGR);
		writer.Write(FileVersion);

		// Write out the set of files
		var files = state.ReferencedFiles;
		writer.Write(FIS);
		writer.Write((uint)files.Count);
		foreach (var file in files)
		{
			// Write the file id + path length + path
			writer.Write(file.FileId.Value);
			WriteValue(writer, file.Path.ToString());
		}

		// Write out the root operation ids
		writer.Write(ROP);
		WriteValues(writer, state.EvaluateGraph.RootOperationIds);

		// Write out the set of operations
		writer.Write(OPS);
		writer.Write((uint)state.EvaluateGraph.Operations.Count);
		foreach (var operationValue in state.EvaluateGraph.Operations)
		{
			OperationGraphWriter.WriteOperationInfo(writer, operationValue.Value);
		}

		// Write out the set of operation proxies
		writer.Write(OPS);
		writer.Write((uint)state.OperationProxies.Count);
		foreach (var operationValue in state.OperationProxies)
		{
			WriteOperationProxyInfo(writer, operationValue.Value);
		}
	}

	private static void WriteOperationProxyInfo(BinaryWriter writer, OperationProxyInfo operationProxy)
	{
		// Write out the operation proxy id
		writer.Write(operationProxy.Id.Value);

		// Write out the operation proxy title
		WriteValue(writer, operationProxy.Title);

		// Write the command working directory
		WriteValue(writer, operationProxy.Command.WorkingDirectory.ToString());

		// Write the command executable
		WriteValue(writer, operationProxy.Command.Executable.ToString());

		// Write the command arguments
		WriteValues(writer, operationProxy.Command.Arguments);

		// Write out the declared input files
		WriteValues(writer, operationProxy.DeclaredInput);

		// Write out the result file
		writer.Write(operationProxy.ResultFile.Value);

		// Write out the finalizer task
		WriteValue(writer, operationProxy.FinalizerTask);

		// Write out the finalizer task
		ValueTableWriter.WriteValueTable(writer, operationProxy.FinalizerState);

		// Write out the read access list
		WriteValues(writer, operationProxy.ReadAccess);
	}

	private static void WriteValue(BinaryWriter writer, string value)
	{
		writer.Write((uint)value.Length);
		writer.Write(value.ToCharArray());
	}

	private static void WriteValues(BinaryWriter writer, IReadOnlyList<string> values)
	{
		writer.Write((uint)values.Count);
		foreach (var value in values)
		{
			writer.Write(value);
		}
	}

	private static void WriteValues(BinaryWriter writer, IList<FileId> values)
	{
		writer.Write((uint)values.Count);
		foreach (var value in values)
		{
			writer.Write(value.Value);
		}
	}

	private static void WriteValues(BinaryWriter writer, IList<OperationId> values)
	{
		writer.Write((uint)values.Count);
		foreach (var value in values)
		{
			writer.Write(value.Value);
		}
	}
}