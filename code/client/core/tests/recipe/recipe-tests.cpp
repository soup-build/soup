// <copyright file="recipe-tests.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <format>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module Soup.Core.Tests:RecipeTests;

import Monitor.Host;
import Opal;
import Soup.Core;
import Soup.SML;
import Soup.Test.Assert;

using namespace Opal;
using namespace Opal::System;
using namespace Soup::SML;
using namespace Soup::Test;

namespace Soup::Core::UnitTests
{
	export class RecipeTests
	{
	public:
		// [[Fact]]
		void InitializerDefault()
		{
			auto uut = Recipe();

			Assert::IsFalse(uut.HasName(), "Verify has no name.");
			Assert::IsFalse(uut.HasLanguage(), "Verify has no language.");
			Assert::IsFalse(uut.HasVersion(), "Verify has no version.");

			Assert::AreEqual(std::vector<std::string>(), uut.GetDependencyTypes(), "Verify no dependency types");
		}

		// [[Fact]]
		void InitializerAll()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::IsTrue(uut.HasName(), "Verify has name.");
			Assert::AreEqual<std::string_view>("my-package", uut.GetName(), "Verify name matches expected.");
			Assert::IsTrue(uut.HasLanguage(), "Verify has language.");
			Assert::AreEqual(LanguageReference("C++", SemanticVersion(1)), uut.GetLanguage(), "Verify language matches expected.");
			Assert::IsTrue(uut.HasVersion(), "Verify has version.");
			Assert::AreEqual(SemanticVersion(1, 2, 3), uut.GetVersion(), "Verify version is correct.");
			
			Assert::AreEqual(
				std::vector<std::string>({ "Runtime", "Build" }),
				uut.GetDependencyTypes(),
				"Verify dependency types");

			Assert::AreEqual(
				std::vector<PackageReference>({
					PackageReference(Path(Path("../other-package/"))),
				}),
				uut.GetNamedDependencies("Runtime"),
				"Verify runtime dependencies are correct.");
			Assert::AreEqual(
				std::vector<PackageReference>({
					PackageReference(Path(Path("../DevTask/"))),
				}),
				uut.GetNamedDependencies("Build"),
				"Verify build dependencies are correct.");
		}

		// [[Fact]]
		void OperatorEqualDefault()
		{
			auto uut = Recipe();

			Assert::AreEqual(Recipe(), uut, "Verify equal.");
		}

		// [[Fact]]
		void OperatorEqualAll()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify equal.");
		}

		// [[Fact]]
		void OperatorNotEqualName()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package2" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualLanguage()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C#|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualVersion()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "2.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualNoVersion()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualRuntimeDependencies()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package2/" }) },
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualNoRuntimeDependencies()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Build", RecipeList({ "../DevTask/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualBuildDependencies()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
							{ "Build", RecipeList({ "../DevTask2/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}

		// [[Fact]]
		void OperatorNotEqualNoBuildDependencies()
		{
			auto uut = Recipe(RecipeTable(
			{
				{ "Name", "my-package" },
				{ "Language", "C++|1" },
				{ "Version", "1.2.3" },
				{
					"Dependencies",
					RecipeTable(
					{
						{ "Runtime", RecipeList({ "../other-package/" }) },
						{ "Build", RecipeList({ "../DevTask/" }) },
					})
				},
			}));

			Assert::AreNotEqual(
				Recipe(RecipeTable(
				{
					{ "Name", "my-package" },
					{ "Language", "C++|1" },
					{ "Version", "1.2.3" },
					{
						"Dependencies",
						RecipeTable(
						{
							{ "Runtime", RecipeList({ "../other-package/" }) },
						})
					},
				})),
				uut,
				"Verify are not equal.");
		}
	};
}
