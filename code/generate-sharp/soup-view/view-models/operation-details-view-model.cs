// <copyright file="operation-details-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Soup.Build.Utilities;
using System.Collections.ObjectModel;
using System.Globalization;
using System.Linq;

namespace Soup.View.ViewModels;

public class OperationDetailsViewModel : ViewModelBase
{
	public OperationDetailsViewModel(
		FileSystemState fileSystemState,
		OperationInfo operation,
		OperationResult? operationResult)
	{
		this.Properties.Clear();
		this.Properties.Add(new PropertyValueViewModel("Title", operation.Title));
		this.Properties.Add(new PropertyValueViewModel("Id", operation.Id.ToString()));
		this.Properties.Add(new PropertyValueViewModel("DependencyCount", operation.DependencyCount.ToString(CultureInfo.InvariantCulture)));
		this.Properties.Add(new PropertyValueViewModel("Executable", operation.Command.Executable.ToString()));
		this.Properties.Add(new PropertyValueViewModel("WorkingDirectory", operation.Command.WorkingDirectory.ToString()));
		this.Properties.Add(new PropertyValueViewModel("Arguments", null)
		{
			Children = [.. operation.Command.Arguments.Select(value => new PropertyValueViewModel(string.Empty, value))],
		});

		var declaredInputFiles = fileSystemState.GetFilePaths(operation.DeclaredInput);
		var declaredOutputFiles = fileSystemState.GetFilePaths(operation.DeclaredOutput);
		var readAccessFiles = fileSystemState.GetFilePaths(operation.ReadAccess);
		var writeAccessFiles = fileSystemState.GetFilePaths(operation.WriteAccess);

		this.Properties.Add(new PropertyValueViewModel("DeclaredInput", null)
		{
			Children = [.. declaredInputFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
		});
		this.Properties.Add(new PropertyValueViewModel("DeclaredOutput", null)
		{
			Children = [.. declaredOutputFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
		});

		this.Properties.Add(new PropertyValueViewModel("ReadAccess", null)
		{
			Children = [.. readAccessFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
		});
		this.Properties.Add(new PropertyValueViewModel("WriteAccess", null)
		{
			Children = [.. writeAccessFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
		});

		if (operationResult != null)
		{
			this.Properties.Add(new PropertyValueViewModel("WasSuccessfulRun", operationResult.WasSuccessfulRun.ToString()));
			this.Properties.Add(new PropertyValueViewModel("EvaluateTime", operationResult.EvaluateTime.ToString(CultureInfo.InvariantCulture)));
			var observedInputFiles = fileSystemState.GetFilePaths(operationResult.ObservedInput);
			var observedOutputFiles = fileSystemState.GetFilePaths(operationResult.ObservedOutput);
			this.Properties.Add(new PropertyValueViewModel("ObservedInput", null)
			{
				Children = [.. observedInputFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
			});
			this.Properties.Add(new PropertyValueViewModel("ObservedOutput", null)
			{
				Children = [.. observedOutputFiles.Select(value => new PropertyValueViewModel(string.Empty, value.ToString()))],
			});
		}
	}

	public ObservableCollection<PropertyValueViewModel> Properties { get; } = [];
}