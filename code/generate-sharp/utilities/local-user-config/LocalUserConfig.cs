﻿// <copyright file="LocalUserConfig.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;

namespace Soup.Build.Utilities;

/// <summary>
/// The local user config container
/// </summary>
public class LocalUserConfig
{
	public static string Property_SDKs => "SDKs";

	/// <summary>
	/// Initializes a new instance of the <see cref="LocalUserConfig"/> class.
	/// </summary>
	public LocalUserConfig()
	{
		this.Document = new SMLDocument();
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="LocalUserConfig"/> class.
	/// </summary>
	public LocalUserConfig(SMLDocument table)
	{
		this.Document = table;
	}

	/// <summary>
	/// Gets or sets the list of SDKs
	/// </summary>
	public bool HasSDKs()
	{
		return this.Document.Values.ContainsKey(Property_SDKs);
	}

	public IList<SDKConfig> GetSDKs()
	{
		if (this.Document.Values.TryGetValue(Property_SDKs, out var sdksValue))
		{
			var values = sdksValue.Value.AsArray();
			var result = new List<SDKConfig>();
			foreach (var value in values.Values)
			{
				result.Add(new SDKConfig(value.Value.AsTable()));
			}

			return result;
		}
		else
		{
			throw new InvalidOperationException("No SDKs.");
		}
	}

	public SDKConfig EnsureSDK(string name)
	{
		// Check the existing entries
		SMLArray? values;
		if (this.Document.Values.TryGetValue(Property_SDKs, out var sdksValue))
		{
			values = sdksValue.Value.AsArray();
			foreach (var value in values.Values)
			{
				var config = new SDKConfig(value.Value.AsTable());
				if (config.HasName() && config.Name == name)
					return config;
			}
		}
		else
		{
			values = this.Document.AddArrayWithSyntax(Property_SDKs);
		}

		// No matching SDK as a table array entry
		var sdkValueTable = values.AddTableWithSyntax(1);

		return new SDKConfig(sdkValueTable)
		{
			Name = name,
		};
	}

	/// <summary>
	/// Raw access
	/// </summary>
	public SMLDocument Document { get; }
}