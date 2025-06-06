// <copyright file="BuildConstants.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

export module Soup.Core:BuildConstants;

import Opal;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// The constants used throughout the build
	/// </summary>
	export class BuildConstants
	{
	public:
		static const Path& EvaluatePhase1ResultsFileName()
		{
			static const auto value = Path("./evaluate-phase1.bor");
			return value;
		}

		static const Path& EvaluatePhase2ResultsFileName()
		{
			static const auto value = Path("./evaluate-phase2.bor");
			return value;
		}

		static const Path& GenerateInputFileName()
		{
			static const auto value = Path("./generate-input.bvt");
			return value;
		}

		static const Path& GeneratePhase1InfoFileName()
		{
			static const auto value = Path("./generate-phase1-info.bvt");
			return value;
		}

		static const Path& GeneratePhase1OperationResultFileName()
		{
			static const auto value = Path("./generate-phase1.bor");
			return value;
		}

		static const Path& GeneratePhase1ResultFileName()
		{
			static const auto value = Path("./generate-phase1.bgr");
			return value;
		}

		static const Path& GeneratePhase2InfoFileName()
		{
			static const auto value = Path("./generate-phase2-info.bvt");
			return value;
		}

		static const Path& GeneratePhase2OperationResultFileName()
		{
			static const auto value = Path("./generate-phase2.bor");
			return value;
		}

		static const Path& GeneratePhase2ResultFileName()
		{
			static const auto value = Path("./generate-phase2.bog");
			return value;
		}

		static const Path& GenerateSharedStateFileName()
		{
			static const auto value = Path("./generate-shared-state.bvt");
			return value;
		}

		static const Path& LocalUserConfigFileName()
		{
			static const auto value = Path("./LocalUserConfig.sml");
			return value;
		}

		static const Path& PackageLockFileName()
		{
			static const auto value = Path("./PackageLock.sml");
			return value;
		}

		static const Path& RecipeFileName()
		{
			static const auto value = Path("./Recipe.sml");
			return value;
		}

		static const Path& SoupLocalStoreDirectory()
		{
			static const auto value = Path("./.soup/");
			return value;
		}

		static const Path& SoupTargetDirectory()
		{
			static const auto value = Path("./.soup/");
			return value;
		}

		static const Path& TemporaryFolderName()
		{
			static const auto value = Path("./temp/");
			return value;
		}
	};
}