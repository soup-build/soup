module;

#ifdef _WIN32
#include <windows.h>

#ifdef SearchPath
#undef SearchPath
#endif
#ifdef CreateProcess
#undef CreateProcess
#endif

#pragma warning(push)
#if _MSC_VER > 1400
#pragma warning(disable:6102 6103)
#endif
#include <strsafe.h>
#pragma warning(pop)

#elif defined(__linux__)

#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <poll.h>
#include <fcntl.h>
#include <cstring>

#include <elf.h>

#include <seccomp.h>
#include "linux/environment.h"

#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <atomic>
#include <array>
#include <codecvt>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

export module Monitor.Host;
#if defined(_WIN32)
import Detours;
#endif
import Monitor.Shared;
import Opal;

using namespace Opal;

#define MONITOR_IMPLEMENTATION

#include "mock/mock-monitor-process-manager.h"
#include "scoped-monitor-process-manager-register.h"

#if defined(_WIN32)
#include "windows/windows-monitor-process-manager.h"
#include "windows/windows-system-logger-monitor.h"
#include "windows/windows-system-monitor-fork.h"
#elif defined(__linux__)
#include "linux/linux-monitor-process-manager.h"
#endif
