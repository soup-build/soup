using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.Notifications;
using Soup.View.ViewModels;

namespace Soup.View;

public partial class MainWindow : Window
{
	public MainWindow()
	{
		InitializeComponent();
		if (this.DataContext is MainWindowViewModel viewModel)
		{
			viewModel.StorageProvider = this.StorageProvider;
		}
	}

	protected override void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs e)
	{
		base.OnAttachedToVisualTree(e);

		if (this.DataContext is MainWindowViewModel viewModel)
		{
			viewModel.NotificationManager = new WindowNotificationManager(GetTopLevel(this)!);
		}
	}
}