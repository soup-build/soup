// <copyright file="main.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

export module Soup.View;

import ftxui;
import Opal;
import Soup.Core;

using namespace Opal;

#include "view-command.h"