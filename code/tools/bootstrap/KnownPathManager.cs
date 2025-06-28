// <copyright file="KnownPathManager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using Opal;
using Opal.System;
using Path = Opal.Path;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Soup.Build.Utilities;
using Soup.Tools;

namespace Soup.Build.Bootstrap;

/// <summary>
/// Manager that replaces known paths with macros
/// </summary>
public class KnownPathManager
{
	// Running State
	private readonly List<(string Path, string Macro)> knownPaths;

	public KnownPathManager(List<(string Path, string Macro)> knownPaths)
	{
		this.knownPaths = knownPaths;
	}

	public string ResolvePath(Path path)
	{
		var pathValue = path.ToString();
		var matchingPath = this.knownPaths.FirstOrDefault(knownPath => pathValue.StartsWith(knownPath.Path));
		if (matchingPath != default)
		{
			pathValue = $"{matchingPath.Macro}{pathValue.Substring(matchingPath.Path.Length)}";
		}

		return pathValue;
	}

	public IList<string> ResolveValues(IReadOnlyList<string> values)
	{
		return values.Select(ResolveValue).ToList();
	}

	private string ResolveValue(string value)
	{
		var matchingPath = this.knownPaths.FirstOrDefault(knownPath => value.Contains(knownPath.Path));
		if (matchingPath != default)
		{
			return value.Replace(matchingPath.Path, matchingPath.Macro);
		}
		else
		{
			return value;
		}
	}
}