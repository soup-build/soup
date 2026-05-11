module;

// TODO: Add a converter level to Opal?
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <any> // IWYU pragma: keep
#include <array> // IWYU pragma: keep
#include <chrono> // IWYU pragma: keep
#include <codecvt> // IWYU pragma: keep
#include <cstring>
#include <ctime>
#include <fstream> // IWYU pragma: keep
#include <iomanip> // IWYU pragma: keep
#include <iostream> // IWYU pragma: keep
#include <locale> // IWYU pragma: keep
#include <map> // IWYU pragma: keep
#include <optional> // IWYU pragma: keep
#include <regex> // IWYU pragma: keep
#include <set> // IWYU pragma: keep
#include <sstream> // IWYU pragma: keep
#include <stack> // IWYU pragma: keep
#include <string> // IWYU pragma: keep
#include <unordered_map> // IWYU pragma: keep
#include <unordered_set> // IWYU pragma: keep
#include <variant> // IWYU pragma: keep
#include <vector> // IWYU pragma: keep

#ifdef _WIN32

#include <format>

#include <Windows.h>
#undef max
#undef min
#undef CreateDirectory
#undef CreateProcess
#undef GetCurrentTime
#undef GetClassName

#elif defined(__linux__)

#include <spawn.h>
#include <sys/wait.h>

#endif

#include <wren/wren.hpp>

export module Soup.Core;

import reflex;
import CryptoPP;
import Monitor.Host;
import Opal;

// Build
export import :BuildConstants;
export import :BuildEvaluateEngine;
import :BuildEvaluateGraphState;
export import :BuildFailedException;
export import :BuildHistoryChecker;
export import :BuildLoadEngine;
export import :BuildRunner;
export import :Build;
export import :DependencyTargetSet;
export import :FileSystemState;
export import :IEvaluateEngine;
export import :KnownLanguage;
export import :MacroManager;
export import :PackageProvider;
export import :RecipeBuildArguments;
export import :RecipeBuildCacheState;
export import :RecipeBuildLocationManager;
export import :SystemAccessTracker;

// Local User Config
export import :LocalUserConfig;
export import :LocalUserConfigExtensions;
export import :SDKConfig;

// Operation Graph
export import :CommandInfo;
export import :GenerateResult;
export import :GenerateResultManager;
export import :GenerateResultReader;
export import :GenerateResultWriter;
export import :OperationGraph;
export import :OperationGraphManager;
export import :OperationGraphReader;
export import :OperationGraphWriter;
export import :OperationInfo;
export import :OperationResult;
export import :OperationResults;
export import :OperationResultsManager;
export import :OperationResultsReader;
export import :OperationResultsWriter;

// Package
export import :PackageManager;

// Package Lock
export import :PackageLock;
export import :PackageLockExtensions;

// Recipe
export import :Digest;
export import :Recipe;
export import :RecipeBuildStateConverter;
export import :RecipeCache;
export import :RecipeExtensions;
export import :RecipeSML;
export import :RecipeValue;
export import :RootRecipe;
export import :RootRecipeExtensions;

// Utilities
export import :HandledException;

// Value Table
export import :Value;
export import :ValueSML;
export import :ValueTableManager;
export import :ValueTableReader;
export import :ValueTableWriter;

// Wren
export import :WrenHelpers;
export import :WrenHost;
export import :WrenValueTable;
