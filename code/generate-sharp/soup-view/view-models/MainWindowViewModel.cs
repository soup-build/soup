﻿using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Controls.Notifications;
using Avalonia.Platform.Storage;
using Avalonia.Threading;
using ReactiveUI;
using Soup.Build.Utilities;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Input;
using Path = Opal.Path;

namespace Soup.View.ViewModels;

public class MainWindowViewModel : ViewModelBase
{
	private readonly FileSystemState fileSystemState = new FileSystemState();

	private Path? recipeFile;
	private readonly DependencyGraphViewModel dependencyGraph;
	private readonly PreprocessorTaskGraphViewModel preprocessorTaskGraph;
	private readonly PreprocessorOperationGraphViewModel preprocessorOperationGraph;
	private readonly TaskGraphViewModel taskGraph;
	private readonly OperationGraphViewModel operationGraph;

	public WindowNotificationManager? NotificationManager { get; set; }

	public IStorageProvider? StorageProvider { get; set; }

	public ICommand OpenCommand { get; }

	public ICommand ExitCommand { get; }

	public ICommand SelectRootCommand { get; }

	public ICommand SelectPreprocessorTasksCommand { get; }

	public ICommand SelectPreprocessorOperationsCommand { get; }

	public ICommand SelectTasksCommand { get; }

	public ICommand SelectOperationsCommand { get; }

	private ViewModelBase content;

	public Path? RecipeFile
	{
		get => this.recipeFile;
		private set
		{
			if (CheckRaiseAndSetIfChanged(ref this.recipeFile, value))
			{
				if (this.recipeFile is not null)
				{
					_ = this.dependencyGraph.LoadProjectAsync(this.recipeFile);
				}
			}
		}
	}

	public ViewModelBase Content
	{
		get => this.content;
		private set
		{
			if (CheckRaiseAndSetIfChanged(ref this.content, value))
			{
				this.RaisePropertyChanged(nameof(this.IsRootSelected));
				this.RaisePropertyChanged(nameof(this.IsPreprocessorTasksSelected));
				this.RaisePropertyChanged(nameof(this.IsPreprocessorOperationsSelected));
				this.RaisePropertyChanged(nameof(this.IsTasksSelected));
				this.RaisePropertyChanged(nameof(this.IsOperationsSelected));
			}
		}
	}

	public MainWindowViewModel(string? packagePath)
	{
		this.OpenCommand = ReactiveCommand.Create(OnOpenAsync);
		this.ExitCommand = ReactiveCommand.Create(OnExit);

		this.SelectRootCommand = ReactiveCommand.Create(OnSelectRoot);
		this.SelectPreprocessorTasksCommand = ReactiveCommand.Create(OnSelectPreprocessorTasks);
		this.SelectPreprocessorOperationsCommand = ReactiveCommand.Create(OnSelectPreprocessorOperations);
		this.SelectTasksCommand = ReactiveCommand.Create(OnSelectTasks);
		this.SelectOperationsCommand = ReactiveCommand.Create(OnSelectOperations);

		this.dependencyGraph = new DependencyGraphViewModel();
		this.preprocessorTaskGraph = new PreprocessorTaskGraphViewModel();
		this.preprocessorOperationGraph = new PreprocessorOperationGraphViewModel(this.fileSystemState);
		this.taskGraph = new TaskGraphViewModel();
		this.operationGraph = new OperationGraphViewModel(this.fileSystemState);

		this.dependencyGraph.PropertyChanged += DependencyGraph_PropertyChanged;

		this.content = this.dependencyGraph;

		if (packagePath is not null)
		{
			this.RecipeFile = Path.Parse(packagePath);
		}
	}

	protected void NotifyError(string message)
	{
		Dispatcher.UIThread.Post(() =>
		{
			this.NotificationManager?.Show(
				new Notification(
					"Error",
					message,
					NotificationType.Error));
		});
	}

	private async void DependencyGraph_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
	{
		if (e.PropertyName == nameof(this.dependencyGraph.SelectedProject))
		{
			var packageState = await LoadPackageAsync(
				this.dependencyGraph.SelectedProject?.Path,
				this.dependencyGraph.SelectedProject?.Owner);

			this.preprocessorTaskGraph.Load(packageState?.GeneratePreprocessorInfo);
			this.preprocessorOperationGraph.Load(packageState?.PreprocessorGraph, packageState?.PreprocessorResults);
			this.taskGraph.Load(packageState?.GenerateEvaluateInfo);
			this.operationGraph.Load(packageState?.EvaluateGraph, packageState?.EvaluateResults);

			this.RaisePropertyChanged(nameof(this.SelectedPackageName));
		}
	}

	public string SelectedPackageName => this.dependencyGraph.SelectedProject?.Name ?? "[Package]";

	public bool IsRootSelected => ReferenceEquals(this.content, this.dependencyGraph);

	public bool IsPreprocessorTasksSelected => ReferenceEquals(this.content, this.preprocessorTaskGraph);

	public bool IsPreprocessorOperationsSelected => ReferenceEquals(this.content, this.preprocessorOperationGraph);

	public bool IsTasksSelected => ReferenceEquals(this.content, this.taskGraph);

	public bool IsOperationsSelected => ReferenceEquals(this.content, this.operationGraph);

	private async Task OnOpenAsync()
	{
		if (this.StorageProvider is null)
			throw new InvalidOperationException("Missing storage provider");

		var filePickerResult = await this.StorageProvider.OpenFilePickerAsync(
			new FilePickerOpenOptions()
			{
				AllowMultiple = false,
				FileTypeFilter =
				[
					new FilePickerFileType("Recipe")
					{
						Patterns = ["Recipe.sml"],
					},
				]
			});

		// Use file picker like normal!
		var file = filePickerResult.Count > 0 ? filePickerResult[0] : null;

		if (file != null)
		{
			this.RecipeFile = new Path(file.Path.AbsolutePath);
		}
	}

	private void OnExit()
	{
		if (Application.Current?.ApplicationLifetime is IClassicDesktopStyleApplicationLifetime desktopApp)
		{
			desktopApp.Shutdown();
		}
	}

	private void OnSelectRoot()
	{
		this.Content = this.dependencyGraph;
	}

	private void OnSelectPreprocessorTasks()
	{
		this.Content = this.preprocessorTaskGraph;
	}

	private void OnSelectPreprocessorOperations()
	{
		this.Content = this.preprocessorOperationGraph;
	}

	private void OnSelectTasks()
	{
		this.Content = this.taskGraph;
	}

	private void OnSelectOperations()
	{
		this.Content = this.operationGraph;
	}

	private async Task<PackageState?> LoadPackageAsync(Path? packageFolder, string? owner)
	{
		if (packageFolder is not null)
		{
			var recipeFile = packageFolder + BuildConstants.RecipeFileName;
			var loadResult = await RecipeExtensions.TryLoadRecipeFromFileAsync(recipeFile);
			if (loadResult.IsSuccess)
			{
				var targetPath = await GetTargetPathAsync(packageFolder, owner);

				var soupTargetDirectory = targetPath + new Path("./.soup/");

				var generatePhase1ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase1ResultFileName;
				if (!GenerateResultManager.TryLoadState(
					generatePhase1ResultFile, this.fileSystemState, out var generatePhase1Result))
				{
					NotifyError($"Failed to load generate phase 1 result: {generatePhase1ResultFile}");
					return null;
				}

				// Assume the generate result is an evaluation result
				OperationGraph? evaluateGraph = generatePhase1Result.EvaluateGraph;
				OperationResults? evaluateResults = null;
				ValueTable? generateEvaluateInfoTable = null;

				// Check for the optional results
				var evaluateBuildResultsFile = soupTargetDirectory + BuildConstants.EvaluateBuildResultsFileName;
				if (OperationResultsManager.TryLoadState(
					evaluateBuildResultsFile, this.fileSystemState, out var loadEvaluateBuildResults))
				{
					evaluateResults = loadEvaluateBuildResults;
				}

				var generatePhase1InfoFile = soupTargetDirectory + BuildConstants.GeneratePhase1InfoFileName;
				if (ValueTableManager.TryLoadState(generatePhase1InfoFile, out var generatePhase1InfoTable))
				{
					generateEvaluateInfoTable = generatePhase1InfoTable;
				}

				// Check for the optional evaluate graph if the initial phase was preprocessor
				OperationGraph? preprocessorGraph = null;
				OperationResults? preprocessorResults = null;
				ValueTable? generatePeprocessorInfoTable = null;
				if (generatePhase1Result.IsPreprocessor)
				{
					// Convert the current graph to preprocessor
					preprocessorGraph = evaluateGraph;
					generatePeprocessorInfoTable = generateEvaluateInfoTable;
					evaluateGraph = null;
					generateEvaluateInfoTable = null;

					var generatePhase2ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase2ResultFileName;
					if (OperationGraphManager.TryLoadState(
						generatePhase2ResultFile, this.fileSystemState, out var loadGeneratePhase2Result))
					{
						evaluateGraph = loadGeneratePhase2Result;
					}

					// Check for the optional preprocessor results
					var evaluateResultsFile = soupTargetDirectory + BuildConstants.EvaluatePreprocessorResultsFileName;
					if (OperationResultsManager.TryLoadState(
						evaluateResultsFile, this.fileSystemState, out var loadEvaluatePreprocessorResults))
					{
						preprocessorResults = loadEvaluatePreprocessorResults;
					}

					var generatePhase2InfoFile = soupTargetDirectory + BuildConstants.GeneratePhase2InfoFileName;
					if (ValueTableManager.TryLoadState(generatePhase2InfoFile, out var generatePhase2InfoTable))
					{
						generateEvaluateInfoTable = generatePhase2InfoTable;
					}
				}

				return new PackageState(
					preprocessorGraph,
					preprocessorResults,
					generatePeprocessorInfoTable,
					evaluateGraph,
					evaluateResults,
					generateEvaluateInfoTable);
			}
			else
			{
				NotifyError($"Failed to load Recipe file: {packageFolder}");
				return null;
			}
		}

		return null;
	}

	private async Task<Path> GetTargetPathAsync(Path packageDirectory, string? owner)
	{
		string soupExe;
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			soupExe = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), "Soup.exe");
		}
		else if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
		{
			soupExe = System.IO.Path.Combine(System.IO.Directory.GetCurrentDirectory(), "soup");
		}
		else
		{
			throw new NotSupportedException("Unknown OS Platform");
		}

		var optionalOwnerFlag = owner is null ? string.Empty : $" -owner {owner}";
		var processInfo = new ProcessStartInfo(soupExe, $"target {packageDirectory}{optionalOwnerFlag}")
		{
			RedirectStandardOutput = true,
			CreateNoWindow = true,
		};
		using var process = new Process()
		{
			StartInfo = processInfo,
		};

		_ = process.Start();

		await process.WaitForExitAsync();

		if (process.ExitCode != 0)
		{
			NotifyError($"Soup process exited with error: {process.ExitCode}");
			throw new InvalidOperationException();
		}

		var output = await process.StandardOutput.ReadToEndAsync();
		return new Path(output);
	}

	private sealed record PackageState(
		OperationGraph? PreprocessorGraph,
		OperationResults? PreprocessorResults,
		ValueTable? GeneratePreprocessorInfo,
		OperationGraph? EvaluateGraph,
		OperationResults? EvaluateResults,
		ValueTable? GenerateEvaluateInfo);
}