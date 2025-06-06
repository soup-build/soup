
using Soup.Build.Utilities;
using Path = Opal.Path;

namespace Soup.View.ViewModels;

public static class DesignData
{
	public static MainWindowViewModel ExampleMainWindow { get; } =
		new MainWindowViewModel(null)
		{
		};

	public static DependencyGraphViewModel ExampleDependencyGraph { get; } =
		new DependencyGraphViewModel()
		{
			SelectedProject = new ProjectDetailsViewModel(
				"TestProject",
				new Path("C:/Test/Project/Recipe.sml"),
				"mwasplund"),
		};

	public static TaskGraphViewModel ExampleTaskGraph { get; } =
		new TaskGraphViewModel()
		{
			SelectedTask = new TaskDetailsViewModel([]),
		};

	public static PreprocessorOperationGraphViewModel ExamplePreprocessorOperationGraph { get; } =
		new PreprocessorOperationGraphViewModel(new FileSystemState())
		{
			SelectedOperation = new OperationDetailsViewModel(
				new FileSystemState(),
				new OperationInfo(
					new OperationId(123),
					"Operation123",
					new CommandInfo(),
					[],
					[],
					[],
					[]),
				null),
		};

	public static PreprocessorTaskGraphViewModel ExamplePreprocessorTaskGraph { get; } =
		new PreprocessorTaskGraphViewModel()
		{
			SelectedTask = new TaskDetailsViewModel([]),
		};

	public static OperationGraphViewModel ExampleOperationGraph { get; } =
		new OperationGraphViewModel(new FileSystemState())
		{
			SelectedOperation = new OperationDetailsViewModel(
				new FileSystemState(),
				new OperationInfo(
					new OperationId(123),
					"Operation123",
					new CommandInfo(),
					[],
					[],
					[],
					[]),
				null),
		};
}