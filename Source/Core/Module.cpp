﻿export module Soup.Core;
// import Soup.BuildEx;
import Soup.Platform;
import Soup.Syntax;
import std.core;
import json11;

#include "Logger\Log.h"
#include "Utils\Helpers.h"
#include "Utils\HandledException.h"

#include "Build\BuildEngine.h"
#include "Build\BuildStateChecker.h"
#include "Build\BuildStateJson.h"
#include "Build\BuildStateManager.h"
#include "Compiler\CompileArguments.h"
#include "Compiler\MockCompiler.h"
#include "Config\LocalUserConfigExtensions.h"
#include "Config\LocalUserConfigJson.h"
#include "Logger\ConsoleTraceListener.h"
#include "Logger\TestTraceListener.h"
#include "Package\Recipe.h"
#include "Package\RecipeBuildManager.h"
#include "Package\RecipeExtensions.h"
#include "Package\RecipeJson.h"
#include "Utils\MockFileSystem.h"
#include "Utils\MockProcessManager.h"
#include "Utils\Path.h"
#include "Utils\PlatformProcessManager.h"
#include "Utils\SemanticVersion.h"
#include "Utils\STLFileSystem.h"
