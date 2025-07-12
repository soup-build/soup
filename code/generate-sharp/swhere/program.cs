// <copyright file="program.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Soup.Build.Utilities;
using System;
using System.Threading.Tasks;

namespace Soup.Build.Discover;

public static class Program
{
	public static async Task<int> Main(string[] args)
	{
		try
		{
			var traceFlags =
				TraceEvents.Information |
				TraceEvents.HighPriority |
				TraceEvents.Critical |
				TraceEvents.Warning |
				TraceEvents.Error;
			Log.RegisterListener(new ConsoleTraceListener(new EventTypeFilter(traceFlags), false, false));
			LifetimeManager.RegisterSingleton<ISystem, RuntimeSystem>();
			LifetimeManager.RegisterSingleton<IFileSystem, RuntimeFileSystem>();
			LifetimeManager.RegisterSingleton<IProcessManager, RuntimeProcessManager>();

			bool includePrerelease = false;
			if (args.Length == 0)
			{
			}
			else if (args.Length == 1 && args[0] == "-prerelease")
			{
				includePrerelease = true;
			}
			else
			{
				PrintUsage();
				return -1;
			}

			if (System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Windows))
			{
				await SwhereManager.DiscoverAsync(OSPlatform.Windows, includePrerelease);
			}
			else if (System.Runtime.InteropServices.RuntimeInformation.IsOSPlatform(System.Runtime.InteropServices.OSPlatform.Linux))
			{
				await SwhereManager.DiscoverAsync(OSPlatform.Linux, includePrerelease);
			}
			else
			{
				throw new NotSupportedException("Unknown OS Platform");
			}

			// Create Root Recipe if missing
			var rootRecipePath = LifetimeManager.Get<IFileSystem>().GetUserProfileDirectory() +
				new Path($"./.soup/{BuildConstants.RootRecipeFileName}");
			Log.Diag("Check Root Recipe: " + rootRecipePath.ToString());
			if (!LifetimeManager.Get<IFileSystem>().Exists(rootRecipePath))
			{
				Log.Warning("Root Recipe file does not exist");
				using var file = LifetimeManager.Get<IFileSystem>().OpenWrite(rootRecipePath, true);
				using var writer = new System.IO.StreamWriter(file.GetOutStream());
				writer.WriteLine("OutputRoot: './out/'");
			}
			return 0;
		}
		catch (HandledException)
		{
			return -1;
		}
	}

	private static void PrintUsage()
	{
		Log.Info("swhere [-prerelease]");
	}
}