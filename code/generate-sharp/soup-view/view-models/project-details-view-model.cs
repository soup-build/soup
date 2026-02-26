// <copyright file="project-details-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.ObjectModel;
using Path = Opal.Path;

namespace Soup.View.ViewModels;

public class ProjectDetailsViewModel : ViewModelBase
{
	public ProjectDetailsViewModel(string name, Path path, string? owner, Path targetDirectory)
	{
		this.Name = name;

		this.Properties.Clear();

		this.Properties.Add(new PropertyValueViewModel("Name", name));
		this.Properties.Add(new PropertyValueViewModel("Path", path.ToString()));
		this.Properties.Add(new PropertyValueViewModel("Owner", owner?.ToString()));
		this.Properties.Add(new PropertyValueViewModel("TargetDirectory", targetDirectory.ToString()));

		this.Path = path;
		this.Owner = owner;
		this.TargetDirectory = targetDirectory;
	}

	public ObservableCollection<PropertyValueViewModel> Properties { get; } = [];

	public string? Name { get; private set; }

	public Path Path { get; private set; }

	public string? Owner { get; private set; }

	public Path TargetDirectory { get; private set; }
}