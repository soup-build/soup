// <copyright file="ValueTable.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Text;

namespace Soup.Build.Utilities;

public class ValueTable : IEnumerable
{
	private Dictionary<string, Value> _impl;

	public ValueTable(IDictionary<string, Value> values)
	{
		_impl = new Dictionary<string, Value>(values);
	}

	public ValueTable()
	{
		_impl = new Dictionary<string, Value>();
	}

	public Value this[string key]
	{
		get { return this._impl[key]; }
		set { this._impl[key] = value; }
	}

	public IEnumerable<string> Keys => this._impl.Keys;

	public IEnumerable<Value> Values => this._impl.Values;

	public int Count => this._impl.Count;

	public void Add(string key, Value value)
	{
		this._impl.Add(key, value);
	}

	public bool ContainsKey(string key)
	{
		return this._impl.ContainsKey(key);
	}

	public ValueTable Clone()
	{
		return new ValueTable(_impl);
	}

	public IEnumerator<KeyValuePair<string, Value>> GetEnumerator()
	{
		return this._impl.GetEnumerator();
	}

	public bool Remove(string key)
	{
		return this._impl.Remove(key);
	}

	public bool TryGetValue(string key, [MaybeNullWhen(false)] out Value value)
	{
		var result = this._impl.TryGetValue(key, out var valueImpl);
		value = valueImpl;
		return result;
	}

	IEnumerator IEnumerable.GetEnumerator()
	{
		return this._impl.GetEnumerator();
	}

	public override string ToString()
	{
		var builder = new StringBuilder();

		builder.Append("{ ");
		foreach (var value in this)
		{
			builder.Append("\"");
			builder.Append(value.Key);
			builder.Append("\": ");
			builder.Append(value.Value);
			builder.Append(", ");
		}

		builder.Append("}");

		return builder.ToString();
	}
}