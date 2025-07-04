
// TODO: Warning unsafe method
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <algorithm>
#include <locale>
#include <codecvt>
#include <format>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

#ifdef _WIN32

#include <windows.h>
#include <wofapi.h>

#ifdef CreateProcess
#undef CreateProcess
#endif

#include <stdio.h>
#include <strsafe.h>

#define DllExport __declspec(dllexport)

#elif defined(__linux__)

#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#endif

import Opal;
import Monitor.Shared;

#ifdef _WIN32

import Detours;

#include "windows/connection-manager.h"
#include "message-sender.h"
#include "windows/dll-main.h"

#elif defined(__linux__)

#include "linux/connection-manager.h"
#include "message-sender.h"
#include "linux/startup.h"

#endif