// <copyright file="value-table-item-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using ReactiveUI;
using System.Collections.ObjectModel;

namespace Soup.View.ViewModels;

public enum ValueTableItemType
{
	Table,
	List,
	Value,
};

public class ValueTableItemViewModel : ViewModelBase
{
	public string Title { get; set; }

	public ValueTableItemType Type { get; set; }
	public ObservableCollection<ValueTableItemViewModel> Children { get; init; } = [];
	public bool IsExpanded
	{
		get;
		set => this.RaiseAndSetIfChanged(ref field, value);
	}
	public bool IsSelected
	{
		get;
		set => this.RaiseAndSetIfChanged(ref field, value);
	}

	public ValueTableItemViewModel(string title)
	{
		this.Title = title;
	}
}