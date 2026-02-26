// <copyright file="main-window-view-model.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Avalonia;
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
		get;
		private set
		{
			if (CheckRaiseAndSetIfChanged(ref field, value))
			{
				if (field is not null)
				{
					_ = this.dependencyGraph.LoadProjectAsync(field);
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

			OperationGraph? generatePreprocessorGraph = null;
			OperationResults? evaluatePreprocessorResults = null;
			ValueTable? generatePreprocessorInfo = null;

			OperationGraph? generateBuildGraph = null;
			OperationResults? evaluateBuildResults = null;
			ValueTable? generateBuildInfo = null;

			if (packageState?.GeneratePhase1Result is not null)
			{
				if (packageState.GeneratePhase1Result.IsPreprocessor)
				{
					// Phase 1 is the preprocessing phase
					generatePreprocessorGraph = packageState.GeneratePhase1Result.EvaluateGraph;
					evaluatePreprocessorResults = packageState.EvaluatePhase1Results;
					generatePreprocessorInfo = packageState.GeneratePhase1Info;

					generateBuildGraph = packageState.GeneratePhase2Result;
					evaluateBuildResults = packageState.EvaluatePhase2Results;
					generateBuildInfo = packageState.GeneratePhase2Info;
				}
				else
				{
					// Phase 1 is normal evaluation
					generateBuildGraph = packageState.GeneratePhase1Result.EvaluateGraph;
					evaluateBuildResults = packageState.EvaluatePhase1Results;
					generateBuildInfo = packageState.GeneratePhase1Info;

				}
			}

			this.preprocessorTaskGraph.Load(generatePreprocessorInfo);
			this.preprocessorOperationGraph.Load(generatePreprocessorGraph, evaluatePreprocessorResults);

			this.taskGraph.Load(generateBuildInfo);
			this.operationGraph.Load(generateBuildGraph, evaluateBuildResults);

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
						Patterns = ["recipe.sml"],
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

				// Check for the optional results
				var evaluatePhase1ResultsFile = soupTargetDirectory + BuildConstants.EvaluatePhase1ResultsFileName;
				OperationResults? evaluatePhase1Results = null;
				if (OperationResultsManager.TryLoadState(
					evaluatePhase1ResultsFile, this.fileSystemState, out var loadEvaluatePhase1Results))
				{
					evaluatePhase1Results = loadEvaluatePhase1Results;
				}

				var generatePhase1InfoFile = soupTargetDirectory + BuildConstants.GeneratePhase1InfoFileName;
				ValueTable? generatePhase1Info = null;
				if (ValueTableManager.TryLoadState(generatePhase1InfoFile, out var loadGeneratePhase1InfoTable))
				{
					generatePhase1Info = loadGeneratePhase1InfoTable;
				}

				// Check for the optional evaluate graph if the initial phase was preprocessor
				OperationGraph? generatePhase2Result = null;
				OperationResults? evaluatePhase2Results = null;
				ValueTable? generatePhase2Info = null;
				if (generatePhase1Result.IsPreprocessor)
				{
					var generatePhase2ResultFile = soupTargetDirectory + BuildConstants.GeneratePhase2ResultFileName;
					if (OperationGraphManager.TryLoadState(
						generatePhase2ResultFile, this.fileSystemState, out var loadGeneratePhase2Result))
					{
						generatePhase2Result = loadGeneratePhase2Result;
					}

					// Check for the optional phase2 results
					var evaluateResultsFile = soupTargetDirectory + BuildConstants.EvaluatePhase2ResultsFileName;
					if (OperationResultsManager.TryLoadState(
						evaluateResultsFile, this.fileSystemState, out var loadEvaluatePreprocessorResults))
					{
						evaluatePhase2Results = loadEvaluatePreprocessorResults;
					}

					var generatePhase2InfoFile = soupTargetDirectory + BuildConstants.GeneratePhase2InfoFileName;
					if (ValueTableManager.TryLoadState(generatePhase2InfoFile, out var generatePhase2InfoTable))
					{
						generatePhase2Info = generatePhase2InfoTable;
					}
				}

				return new PackageState(
					generatePhase1Result,
					evaluatePhase1Results,
					generatePhase1Info,
					generatePhase2Result,
					evaluatePhase2Results,
					generatePhase2Info);
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
		GenerateResult? GeneratePhase1Result,
		OperationResults? EvaluatePhase1Results,
		ValueTable? GeneratePhase1Info,
		OperationGraph? GeneratePhase2Result,
		OperationResults? EvaluatePhase2Results,
		ValueTable? GeneratePhase2Info);
}