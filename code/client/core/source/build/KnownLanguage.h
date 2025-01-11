﻿// <copyright file="KnownLanguage.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once

namespace Soup::Core
{
	export struct KnownLanguage
	{
		KnownLanguage(
			std::string extensionOwner,
			std::string extensionName) :
			ExtensionOwner(std::move(extensionOwner)),
			ExtensionName(std::move(extensionName))
		{
		}

		std::string ExtensionOwner;
		std::string ExtensionName;
	};
}
