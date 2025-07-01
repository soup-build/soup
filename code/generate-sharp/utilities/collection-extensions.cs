// <copyright file="collection-extensions.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections.Generic;

namespace Soup;

public static class CollectionExtensions
{
	public static void AddRange<T>(this IList<T> list, IEnumerable<T> range)
	{
		foreach (var value in range)
		{
			list.Add(value);
		}
	}
}