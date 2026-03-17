// <copyright file="package-identifier-unittests.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Xunit;

namespace Soup.Build.Utilities.UnitTests;

public class PackageIdentifierUnitTests
{
	[Fact]
	public void DefaultInitializer()
	{
		var uut = new PackageIdentifier();
		Assert.Null(uut.Language);
		Assert.Null(uut.Owner);
		Assert.Equal(string.Empty, uut.Name);
	}

	[Fact]
	public void Parse_Public()
	{
		var uut = PackageIdentifier.Parse("other");
		Assert.Null(uut.Language);
		Assert.Null(uut.Owner);
		Assert.Equal("other", uut.Name);
	}

	[Fact]
	public void Parse_Public_Owner()
	{
		var uut = PackageIdentifier.Parse("user1|other");
		Assert.Null(uut.Language);
		Assert.Equal("user1", uut.Owner);
		Assert.Equal("other", uut.Name);
	}

	[Fact]
	public void Parse_Public_Owner_Language()
	{
		var uut = PackageIdentifier.Parse("[C#]user1|other");
		Assert.Equal("C#", uut.Language);
		Assert.Equal("user1", uut.Owner);
		Assert.Equal("other", uut.Name);
	}
}