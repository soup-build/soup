// <copyright file="package-name-unittests.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Xunit;

namespace Soup.Build.Utilities.UnitTests;

public class PackageNameUnitTests
{
	[Fact]
	public void DefaultInitializer()
	{
		var uut = new PackageName(null, string.Empty);
		Assert.Null(uut.Owner);
		Assert.Equal("", uut.Name);
	}

	[Fact]
	public void Parse_Public()
	{
		var uut = PackageName.Parse("other");
		Assert.Null(uut.Owner);
		Assert.Equal("other", uut.Name);
	}

	[Fact]
	public void Parse_Public_Owner()
	{
		var uut = PackageName.Parse("user1|other");
		Assert.Equal("user1", uut.Owner);
		Assert.Equal("other", uut.Name);
	}
}