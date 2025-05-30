module;

// TODO: Add a converter level to Opal?
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <any>
#include <array>
#include <chrono>
#include <codecvt>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <regex>
#include <optional>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

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

// TODO module
// TODO: Treat wren as C code
#include "wren/wren.h"

export module Soup.Core;

import reflex;
import CryptoPP;
import Monitor.Host;
import Opal;

// Build
export import :BuildConstants;
export import :BuildFailedException;
export import :BuildHistoryChecker;
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
export import :LanguageReference;
export import :PackageIdentifier;
export import :PackageName;
export import :PackageReference;
export import :Recipe;
export import :RecipeBuildStateConverter;
export import :RecipeCache;
export import :RecipeExtensions;
export import :RecipeSML;
export import :RecipeValue;
export import :RootRecipe;
export import :RootRecipeExtensions;

// SML
export import :SML;

// Utilities
export import :HandledException;
export import :SequenceMap;

// Value Table
export import :Value;
export import :ValueTableManager;
export import :ValueTableReader;
export import :ValueTableWriter;

// Wren
export import :WrenHelpers;
export import :WrenHost;
export import :WrenValueTable;

using namespace Opal;

#include "build/BuildEngine.h"