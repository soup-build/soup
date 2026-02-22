// <copyright file="task-details-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Soup.Build.Utilities;
using System;
using System.Collections.ObjectModel;
using ValueType = Soup.Build.Utilities.ValueType;

namespace Soup.View.ViewModels;

public class TaskDetailsViewModel : ViewModelBase
{
	public TaskDetailsViewModel(ValueTable taskInfo)
	{
		this.Properties.Clear();
		BuildValueTable(taskInfo, this.Properties);
	}

	public ObservableCollection<ValueTableItemViewModel> Properties { get; } = [];

	private void BuildValueTable(
		ValueTable table,
		ObservableCollection<ValueTableItemViewModel> viewModelList)
	{
		viewModelList.Clear();
		foreach (var value in table)
		{
			string title;
			ValueTableItemType type;
			var children = new ObservableCollection<ValueTableItemViewModel>();
			switch (value.Value.Type)
			{
				case ValueType.Boolean:
					title = $"{value.Key}: {value.Value.AsBoolean()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.String:
					title = $"{value.Key}: \"{value.Value.AsString()}\"";
					type = ValueTableItemType.Value;
					break;
				case ValueType.Float:
					title = $"{value.Key}: {value.Value.AsFloat()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.Integer:
					title = $"{value.Key}: {value.Value.AsInteger()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.List:
					title = $"{value.Key}";
					type = ValueTableItemType.List;
					BuildValueList(value.Value.AsList(), children);
					break;
				case ValueType.Table:
					title = $"{value.Key}";
					type = ValueTableItemType.Table;
					BuildValueTable(value.Value.AsTable(), children);
					break;
				case ValueType.Version:
					title = $"{value.Key}: {value.Value}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.PackageReference:
					title = $"{value.Key}: {value.Value}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.LanguageReference:
					title = $"{value.Key}: {value.Value}";
					type = ValueTableItemType.Value;
					break;
				default:
					throw new InvalidOperationException("Unknown Value type");
			}

			var viewModel = new ValueTableItemViewModel(title)
			{
				Type = type,
				Children = children,
			};

			viewModelList.Add(viewModel);
		}
	}

	private void BuildValueList(
		ValueList list,
		ObservableCollection<ValueTableItemViewModel> viewModelList)
	{
		viewModelList.Clear();
		foreach (var value in list)
		{
			string title;
			ValueTableItemType type;
			var children = new ObservableCollection<ValueTableItemViewModel>(); ;
			switch (value.Type)
			{
				case ValueType.Boolean:
					title = $"{value.AsBoolean()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.String:
					title = $"\"{value.AsString()}\"";
					type = ValueTableItemType.Value;
					break;
				case ValueType.Float:
					title = $"{value.AsFloat()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.Integer:
					title = $"{value.AsInteger()}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.List:
					title = string.Empty;
					type = ValueTableItemType.List;
					BuildValueList(value.AsList(), children);
					break;
				case ValueType.Table:
					title = string.Empty;
					type = ValueTableItemType.Table;
					BuildValueTable(value.AsTable(), children);
					break;
				case ValueType.Version:
					title = $"{value}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.PackageReference:
					title = $"{value}";
					type = ValueTableItemType.Value;
					break;
				case ValueType.LanguageReference:
					title = $"{value}";
					type = ValueTableItemType.Value;
					break;
				default:
					throw new InvalidOperationException("Unknown Value type");
			}

			var viewModel = new ValueTableItemViewModel(title)
			{
				Type = type,
				Children = children,
			};

			viewModelList.Add(viewModel);
		}
	}
}