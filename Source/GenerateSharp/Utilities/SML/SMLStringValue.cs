﻿// <copyright file="SMLStringValue.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;

namespace Soup.Build.Utilities;

public class SMLStringValue : IEquatable<SMLStringValue>
{
	public string Value { get; set; }

	public SMLToken OpenQuote { get; set; }

	public SMLToken Content { get; set; }

	public SMLToken CloseQuote { get; set; }

	public SMLStringValue()
	{
		Value = string.Empty;
		OpenQuote = SMLToken.Empty;
		Content = SMLToken.Empty;
		CloseQuote = SMLToken.Empty;
	}

	public SMLStringValue(string content)
	{
		Value = content;
		OpenQuote = SMLToken.Empty;
		Content = new SMLToken(content);
		CloseQuote = SMLToken.Empty;
	}

	public SMLStringValue(
		string value,
		SMLToken openQuote,
		SMLToken content,
		SMLToken closeQuote)
	{
		Value = value;
		OpenQuote = openQuote;
		Content = content;
		CloseQuote = closeQuote;
	}

	public override bool Equals(object? obj) => this.Equals(obj as SMLStringValue);

	public bool Equals(SMLStringValue? rhs)
	{
		if (rhs is null)
			return false;

		// Optimization for a common success case.
		if (object.ReferenceEquals(this, rhs))
			return true;

		// Return true if the fields match.
		return this.Value == rhs.Value;
	}

	public override int GetHashCode() => (Value).GetHashCode();

	public static bool operator ==(SMLStringValue? lhs, SMLStringValue? rhs)
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

	public static bool operator !=(SMLStringValue? lhs, SMLStringValue? rhs) => !(lhs == rhs);
}