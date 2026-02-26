// <copyright file="digest.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Diagnostics.CodeAnalysis;

namespace Opal;

/// <summary>
/// The digest class.
/// </summary>
public class Digest : IEquatable<Digest>
{
	/// <summary>
	/// Gets or sets the function.
	/// </summary>
	public required string Function { get; set; }

	/// <summary>
	/// Gets or sets the hash.
	/// </summary>
	public required string Hash { get; set; }

	/// <summary>
	/// Try parse the value.
	/// </summary>
	/// <param name="value">The value.</param>
	/// <param name="result">The output parameter result.</param>
	public static bool TryParse(string value, [MaybeNullWhen(false)] out Digest result)
	{
		result = null;

		// Parse the values
		var stringValues = value.Split(':');
		if (stringValues.Length != 2)
		{
			return false;
		}

		var function = stringValues[0];
		var hash = stringValues[1];

		result = new Digest()
		{
			Function = function,
			Hash = hash,
		};
		return true;
	}

	/// <summary>
	/// Parse the value.
	/// </summary>
	/// <param name="value">The value.</param>
	public static Digest Parse(string value)
	{
		if (TryParse(value, out var result))
		{
			return result;
		}
		else
		{
			throw new ArgumentException("Invalid digest");
		}
	}

	public bool Equals(Digest? other)
	{
		if (other is null)
			return false;
		return this.Function == other.Function &&
			this.Hash == other.Hash;
	}

	public override bool Equals(object? obj)
	{
		return Equals(obj as Digest);
	}

	public override int GetHashCode()
	{
		return this.Hash.GetHashCode(StringComparison.OrdinalIgnoreCase);
	}

	public static bool operator ==(Digest? lhs, Digest? rhs)
	{
		if (lhs is null)
			return rhs is null;
		return lhs.Equals(rhs);
	}

	public static bool operator !=(Digest? lhs, Digest? rhs)
	{
		return !(lhs == rhs);
	}

	/// <summary>
	/// Convert to string.
	/// </summary>
	public override string ToString()
	{
		return $"{this.Function}:{this.Hash}";
	}
}