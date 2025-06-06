﻿// <copyright file="MockLibrary.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

namespace Opal.System;

/// <summary>
/// A mock library executable using system.
/// </summary>
public class MockLibrary : ILibrary
{
	// Input
	//private readonly string name;
	//private readonly IList<string> requests;

	// std::map<std::string, void*> _functions;

	/// <summary>
	/// Initializes a new instance of the <see cref='MockLibrary'/> class.
	/// </summary>
	public MockLibrary()
	{
		//this.name = name;
		//this.requests = requests;
	}

	///// <summary>
	///// Register a known function pointer
	///// </summary>
	////public void RegisterFunction(std::string name, void* function)
	////{
	////	_functions.emplace(
	////		std::move(name),
	////		function);
	////}

	///// <summary>
	///// Get a function by name and casted to the requested type
	///// </summary>
	////void* GetFunction(std::string_view name) override final
	////{
	////	std::stringStream message;
	////	message << "GetFunction: " << m_name << " [" << name << "]";

	////	m_requests.push_back(message.str());

	////	auto function = _functions.find(name.data());
	////	if (function != _functions.end())
	////	{
	////		return function->second;
	////	}
	////	else
	////	{
	////		auto errorMessage = "Cannot find function: " + std::string(name);
	////		throw std::runtime_error(errorMessage);
	////	}
	////}
}