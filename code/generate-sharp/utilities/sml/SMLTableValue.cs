﻿// <copyright file="SMLTableValue.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Collections.Generic;

namespace Soup.Build.Utilities;

public class SMLTableValue : IEquatable<SMLTableValue>
{
	public SMLToken Key { get; init; }
	public string KeyContent { get; init; }
	public SMLToken Colon { get; init; }
	public SMLValue Value { get; init; }
	public IList<SMLToken> Delimiter { get; init; }

	public SMLTableValue(
		SMLToken key,
		string keyContent,
		SMLToken colon,
		SMLValue value)
	{
		this.Key = key;
		this.KeyContent = keyContent;
		this.Colon = colon;
		this.Value = value;
		this.Delimiter = [];
	}

	public SMLTableValue(
		SMLToken key,
		string keyContent,
		SMLToken colon,
		SMLValue value,
		IList<SMLToken> delimiter)
	{
		this.Key = key;
		this.KeyContent = keyContent;
		this.Colon = colon;
		this.Value = value;
		this.Delimiter = delimiter;
	}

	public override bool Equals(object? obj)
	{
		return Equals(obj as SMLTableValue);
	}

	public bool Equals(SMLTableValue? other)
	{
		if (other is null)
			return false;

		// Optimization for a common success case.
		if (ReferenceEquals(this, other))
			return true;

		// Return true if the fields match.
		return this.KeyContent == other.KeyContent &&
			this.Value == other.Value;
	}

	public override int GetHashCode()
	{
		return this.Value.GetHashCode();
	}

	public static bool operator ==(SMLTableValue? lhs, SMLTableValue? rhs)
	{
		if (lhs is null)
		{
			if (rhs is null)
				return true;
			else
				return false;
		}

		return lhs.Equals(rhs);
	}

	public static bool operator !=(SMLTableValue? lhs, SMLTableValue? rhs)
	{
		return !(lhs == rhs);
	}
}