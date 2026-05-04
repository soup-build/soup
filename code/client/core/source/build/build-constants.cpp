// <copyright file="build-constants.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;
#include <cmath>
#include <thread>
export module Soup.Core:BuildConstants;
import Opal;

using namespace Opal;

export namespace Soup::Core::Build::Constants
{
	const Path& EvaluatePhase1ResultsFileName()
	{
		static const auto value = Path("./evaluate-phase1.bor");
		return value;
	}

	const Path& EvaluatePhase2ResultsFileName()
	{
		static const auto value = Path("./evaluate-phase2.bor");
		return value;
	}

	const Path& GenerateInputFileName()
	{
		static const auto value = Path("./generate-input.bvt");
		return value;
	}

	const Path& GeneratePhase1InfoFileName()
	{
		static const auto value = Path("./generate-phase1-info.bvt");
		return value;
	}

	const Path& GeneratePhase1OperationResultFileName()
	{
		static const auto value = Path("./generate-phase1.bor");
		return value;
	}

	const Path& GeneratePhase1ResultFileName()
	{
		static const auto value = Path("./generate-phase1.bgr");
		return value;
	}

	const Path& GeneratePhase2InfoFileName()
	{
		static const auto value = Path("./generate-phase2-info.bvt");
		return value;
	}

	const Path& GeneratePhase2OperationResultFileName()
	{
		static const auto value = Path("./generate-phase2.bor");
		return value;
	}

	const Path& GeneratePhase2ResultFileName()
	{
		static const auto value = Path("./generate-phase2.bog");
		return value;
	}

	const Path& GenerateSharedStateFileName()
	{
		static const auto value = Path("./generate-shared-state.bvt");
		return value;
	}

	const Path& LocalUserConfigFileName()
	{
		static const auto value = Path("./local-user-config.sml");
		return value;
	}

	const Path& PackageLockFileName()
	{
		static const auto value = Path("./package-lock.sml");
		return value;
	}

	const Path& RecipeFileName()
	{
		static const auto value = Path("./recipe.sml");
		return value;
	}

	const Path& RootRecipeFileName()
	{
		static const auto value = Path("./root-recipe.sml");
		return value;
	}

	const Path& SoupLocalStoreDirectory()
	{
		static const auto value = Path("./.soup/");
		return value;
	}

	const Path& SoupTargetDirectory()
	{
		static const auto value = Path("./.soup/");
		return value;
	}

	const Path& TemporaryFolderName()
	{
		static const auto value = Path("./temp/");
		return value;
	}

	// TODO: Find a better place for this
	Path GetSoupUserDataPath()
	{
		auto result = System::IFileSystem::Current().GetUserProfileDirectory() +
			SoupLocalStoreDirectory();
		return result;
	}

	unsigned int GetDefaultParallelization()
	{
		auto threadCount = std::thread::hardware_concurrency();
		if (threadCount > 0)
		{
			// Leave a few cores so the system stays responsive
			return  static_cast<unsigned int>(std::floor(threadCount * 0.75));
		}
		else
		{
			// Safe small number
			return 2;
		}
	}
}