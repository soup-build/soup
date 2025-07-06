using System;
using WixSharp;

namespace SoupInstaller;

public static class Script
{
	public static void Main()
	{
		var soupVersion = new Version(0, 42, 4);

		var soupOutFolder = "../../../out";
		var soupReleaseFolder = $"{soupOutFolder}/release";
		var soupRunFolder = $"{soupOutFolder}/run";
		var project = new Project(
			"Soup Build",
			new Dir(
				@"%ProgramFiles%\SoupBuild\Soup",
				new Files($"{soupRunFolder}/*.*")),
			new EnvironmentVariable("PATH", "[INSTALLDIR]")
			{
				System = true,
				Permanent = false,
				Part = EnvVarPart.last,
			})
		{
			Platform = Platform.x64,
			GUID = new Guid("2CF2A792-A266-416D-ACF9-B81781DC2B76"),
			BackgroundImage = @"images\dialog-background.bmp",
			BannerImage = @"images\dialog-banner.bmp",
			LicenceFile = @"license.rtf",
			OutDir = soupReleaseFolder,
			OutFileName = $"soup-build-{soupVersion}-windows-x64",

			// Prevent two installations at the same time
			MajorUpgrade = new MajorUpgrade()
			{
				Schedule = UpgradeSchedule.afterInstallInitialize,
				DowngradeErrorMessage = "A later version of [ProductName] is already installed. Setup will now exit.",
			},

			// Upgrade values
			Version = soupVersion
		};

		var msiPath = Compiler.BuildMsi(project);
		Console.WriteLine($"MSI Created: {msiPath}");
	}
}