// <copyright file="RecipeBuildCacheState.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <map>
#include <set>
#include <string>

export module Soup.Core:RecipeBuildCacheState;

import Opal;

using namespace Opal;

namespace Soup::Core
{
	class RecipeBuildCacheState
	{
	public:
		RecipeBuildCacheState(
			std::string name,
			Path macroTargetDirectory,
			Path realTargetDirectory,
			Path soupTargetDirectory,
			std::set<Path> recursiveChildMacroTargetDirectorySet,
			std::map<std::string, std::string> recursiveChildMacros) :
			Name(std::move(name)),
			MacroTargetDirectory(std::move(macroTargetDirectory)),
			RealTargetDirectory(std::move(realTargetDirectory)),
			SoupTargetDirectory(std::move(soupTargetDirectory)),
			RecursiveChildMacroTargetDirectorySet(std::move(recursiveChildMacroTargetDirectorySet)),
			RecursiveChildMacros(std::move(recursiveChildMacros))
		{
		}

		std::string Name;
		Path MacroTargetDirectory;
		Path RealTargetDirectory;
		Path SoupTargetDirectory;
		std::set<Path> RecursiveChildMacroTargetDirectorySet;
		std::map<std::string, std::string> RecursiveChildMacros;
	};
}
