// <copyright file="GenerateResultManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Text;
using Path = Opal.Path;

namespace Soup.Build.Utilities;

/// <summary>
/// The generate result manager
/// </summary>
public static class GenerateResultManager
{
	/// <summary>
	/// Load the generate result from the provided directory
	/// </summary>
	public static bool TryLoadState(
		Path GenerateResultFile,
		FileSystemState fileSystemState,
		[MaybeNullWhen(false)] out GenerateResult result)
	{
		// Verify the requested file exists
		if (!LifetimeManager.Get<IFileSystem>().Exists(GenerateResultFile))
		{
			Log.Info("Generate result file does not exist");
			result = null;
			return false;
		}

		// Open the file to read from
		using var file = LifetimeManager.Get<IFileSystem>().OpenRead(GenerateResultFile);
		using var reader = new System.IO.BinaryReader(file.GetInStream(), Encoding.UTF8, true);

		// Read the contents of the build state file
		try
		{
			var loadedResult = GenerateResultReader.Deserialize(reader);

			// Map up the incoming file ids to the active file system state ids
			var activeFileIdMap = new Dictionary<FileId, FileId>();
			for (var i = 0; i < loadedResult.ReferencedFiles.Count; i++)
			{
				var fileReference = loadedResult.ReferencedFiles[i];
				var activeFileId = fileSystemState.ToFileId(fileReference.Path);
				activeFileIdMap.Add(fileReference.FileId, activeFileId);

				// Update the referenced id
				fileReference.FileId = activeFileId;
			}

			// Update all of the operations
			foreach (var operationReference in loadedResult.EvaluateGraph.Operations)
			{
				var operation = operationReference.Value;
				UpdateFileIds(operation.DeclaredInput, activeFileIdMap);
				UpdateFileIds(operation.DeclaredOutput, activeFileIdMap);
			}

			foreach (var operationReference in loadedResult.OperationProxies)
			{
				var operation = operationReference.Value;
				UpdateFileIds(operation.DeclaredInput, activeFileIdMap);
				operation.ResultFile = UpdateFileId(operation.ResultFile, activeFileIdMap);
			}

			result = loadedResult;
			return true;
		}
		catch (InvalidOperationException)
		{
			Log.Error("Failed to parse operation graph");
			result = null;
			return false;
		}
	}

	/// <summary>
	/// Save the operation state for the provided directory
	/// </summary>
	public static void SaveState(
		Path GenerateResultFile,
		GenerateResult state,
		FileSystemState fileSystemState)
	{
		// Update the operation graph referenced files
		var files = new HashSet<FileId>();
		foreach (var operationReference in state.EvaluateGraph.Operations)
		{
			var operation = operationReference.Value;
			files.UnionWith(operation.DeclaredInput);
			files.UnionWith(operation.DeclaredOutput);
			files.UnionWith(operation.ReadAccess);
			files.UnionWith(operation.WriteAccess);
		}

		foreach (var operationProxyReference in state.OperationProxies)
		{
			var operationProxy = operationProxyReference.Value;
			files.UnionWith(operationProxy.DeclaredInput);
			_ = files.Add(operationProxy.ResultFile);
			files.UnionWith(operationProxy.ReadAccess);
		}

		var referencedFiles = new List<(FileId FileId, Path Path)>();
		foreach (var fileId in files)
		{
			referencedFiles.Add((fileId, fileSystemState.GetFilePath(fileId)));
		}

		state.ReferencedFiles.Clear();
		state.ReferencedFiles.AddRange(referencedFiles);

		// Open the file to write to
		using var fileStream = System.IO.File.Open(
			GenerateResultFile.ToString(),
			System.IO.FileMode.Create,
			System.IO.FileAccess.Write);
		using var writer = new System.IO.BinaryWriter(fileStream);

		// Write the build state to the file stream
		GenerateResultWriter.Serialize(state, writer);
	}

	private static void UpdateFileIds(IList<FileId> fileIds, Dictionary<FileId, FileId> activeFileIdMap)
	{
		for (var i = 0; i < fileIds.Count; i++)
		{
			var findActiveFileId = activeFileIdMap[fileIds[i]];
			fileIds[i] = findActiveFileId;
		}
	}

	private static FileId UpdateFileId(FileId fileId, Dictionary<FileId, FileId> activeFileIdMap)
	{
		var findActiveFileId = activeFileIdMap[fileId];
		return findActiveFileId;
	}
}