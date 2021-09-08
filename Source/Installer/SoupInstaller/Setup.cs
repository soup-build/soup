﻿using System;
using WixSharp;

class Script
{
	static public void Main()
	{
		var soupBinFolder = @"..\..\..\out\C++\Soup\1281a639dd5d393781f4188942c6bc05544b7c2c0124eae4cc59236248e36\bin\";
		var soupBinGenerateFolder = @"..\..\..\out\msbuild\bin\Soup.Build.Generate\Release\net5.0\";
		var soupBinPackageManagerFolder = @"..\..\..\out\msbuild\Soup.Build.PackageManager\Release\net5-windows10.0.17763.0\";
		var project = new Project(
			"Soup Build",
			new Dir(
				@"%ProgramFiles%\SoupBuild\Soup",
				new DirFiles(System.IO.Path.Combine(soupBinFolder, "*.*")),
				new Dir(
					@"Generate",
					new DirFiles(System.IO.Path.Combine(soupBinGenerateFolder, "*.*")),
					new Dir(
						@"Extensions",
						new Dir(
							@"Soup.Cpp",
							new DirFiles(System.IO.Path.Combine(soupBinGenerateFolder, @"Extensions\Soup.Cpp\", "*.*"))),
						new Dir(
							@"Soup.CSharp",
							new DirFiles(System.IO.Path.Combine(soupBinGenerateFolder, @"Extensions\Soup.CSharp\", "*.*"))))),
				new Dir(
					@"PackageManager",
					new DirFiles(System.IO.Path.Combine(soupBinPackageManagerFolder, "*.*")))),
			new EnvironmentVariable("PATH", "[INSTALLDIR]")
			{
				System = true,
				Permanent = false,
				Part = EnvVarPart.last,
			});

		project.Platform = Platform.x64;
		project.GUID = new Guid("2CF2A792-A266-416D-ACF9-B81781DC2B76");
		project.BackgroundImage = @"Images\DialogBackground.bmp";
		project.BannerImage = @"Images\DialogBanner.bmp";
		project.LicenceFile = @"License.rtf";
		project.OutDir = @"msi";

		// Prevent two installations at the same time
		project.MajorUpgrade = new MajorUpgrade()
		{
			Schedule = UpgradeSchedule.afterInstallInitialize,
			DowngradeErrorMessage = "A later version of [ProductName] is already installed. Setup will now exit.",
		};

		// Upgrade values
		project.Version = new Version(0, 12, 7);

		Compiler.BuildMsi(project);
	}
}