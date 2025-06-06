// <copyright file="PreprocessorOperationGraphView.axaml.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Notifications;
using Soup.View.ViewModels;

namespace Soup.View.Views;

/// <summary>
/// The operation graph page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class PreprocessorOperationGraphView : UserControl
{
	public PreprocessorOperationGraphView()
	{
		InitializeComponent();
	}

	protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
	{
		base.OnAttachedToVisualTree(e);

		var dataContext = (ContentPaneViewModel?)this.DataContext;
		if (dataContext is not null)
		{
			dataContext.NotificationManager = new WindowNotificationManager(TopLevel.GetTopLevel(this)!);
		}
	}
}