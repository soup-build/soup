module;
#include <optional>
#include <stdexcept>
export module BuildSystem:BuildSystemMonitor;
import Opal;
import Soup.Core;
import Soup.SML;

using namespace Opal;

namespace BuildSystem {
	export class BuildSystemMonitor {
	private:
		Soup::Core::FileSystemState _fileSystemState;

	public:
		BuildSystemMonitor()
			: _fileSystemState() {
		}

		std::optional<Soup::Core::OperationInfo> TryFindFileOperationInfo(const Path &file) {
			Log::Info("TryFindFileOperationInfo");
			auto packageRoot = TryFindPackageRoot(file);
			if (packageRoot) {
				auto targetDirectory = GetTargetDirectory(*packageRoot);
				auto operationGraph = TryLoadPackageOperationGraph(targetDirectory);
				if (operationGraph) {
					for (auto &[operationId, operationInfo] : operationGraph->GetOperations()) {
						for (auto &inputFileId : operationInfo.DeclaredInput) {
							auto inputFile = _fileSystemState.GetFilePath(inputFileId);
							if (inputFile == file) {
								return operationInfo;
							}
						}
					}
				}
			}
			return {};
		}

	private:
		std::optional<Path> TryFindPackageRoot(const Path &file) {
			Log::Info("TryFindPackageRoot");
			auto currentDirectory = file.HasFileName() ? file.GetParent() : file;
			while (!currentDirectory.IsEmpty()) {
				auto testRecipe = currentDirectory + Soup::Core::Build::Constants::RecipeFileName();
				if (Opal::System::IFileSystem::Current().Exists(testRecipe)) {
					return currentDirectory;
				}

				currentDirectory = currentDirectory.GetParent();
			}

			return {};
		}

		Path GetTargetDirectory(const Path &workingDirectory) {
			Log::Info("GetTargetDirectory");
			// Load the recipe
			auto recipeCache = Soup::Core::RecipeCache();
			auto recipePath = workingDirectory + Soup::Core::Build::Constants::RecipeFileName();
			const Soup::Core::Recipe *recipe;
			if (!recipeCache.TryGetOrLoadRecipe(recipePath, recipe)) {
				Log::Error("The Recipe does not exist: {}", recipePath.ToString());
				Log::HighPriority("Make sure the path is correct and try again");
				throw std::runtime_error("failed to load recipe");
			}

			// Build up the unique name
			auto packageName = Soup::SML::PackageName(std::nullopt, recipe->GetName());

			// Setup the build parameters
			auto globalParameters = Soup::Core::ValueTable();

			// Process well known parameters
			// if (!_options.Flavor.empty())
			//	globalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			// if (!_options.Architecture.empty())
			//	globalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			// TODO: Generic parameters

			// Load the value table to get the exe path
			auto knownLanguages = Soup::Core::Build::GetKnownLanguages();
			auto locationManager = Soup::Core::RecipeBuildLocationManager(knownLanguages);
			auto targetDirectory = locationManager.GetOutputDirectory(
				packageName, workingDirectory, *recipe, globalParameters, recipeCache);

			return targetDirectory;
		}

		std::optional<Soup::Core::OperationGraph> TryLoadPackageOperationGraph(
			Path &targetDirectory) {
			auto soupTargetDirectory = targetDirectory + Path("./.soup/");

			auto generatePhase1ResultFile =
				soupTargetDirectory + Soup::Core::Build::Constants::GeneratePhase1ResultFileName();
			auto generatePhase1Result = Soup::Core::GenerateResult();
			bool generatePhase1IsPreprocessor = false;
			if (Soup::Core::GenerateResultManager::TryLoadState(
					generatePhase1ResultFile, generatePhase1Result, _fileSystemState)) {
				generatePhase1IsPreprocessor = generatePhase1Result.HasPreprocessor();
				if (generatePhase1IsPreprocessor) {
					// Check for the optional evaluate graph if the initial phase was
					// preprocessor
					auto generatePhase2ResultFile =
						soupTargetDirectory +
						Soup::Core::Build::Constants::GeneratePhase2ResultFileName();
					auto generatePhase2Result = Soup::Core::OperationGraph();
					if (Soup::Core::OperationGraphManager::TryLoadState(
							generatePhase2ResultFile, generatePhase2Result, _fileSystemState)) {
						return generatePhase2Result;
					}
				} else {
					return generatePhase1Result.GetGraph();
				}
			}

			return std::nullopt;
		}
	};
}
