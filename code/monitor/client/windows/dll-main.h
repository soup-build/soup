#pragma once

#include "helpers.h"

#include "functions/overrides/file-api.h"
#include "functions/overrides/lib-loader-api.h"
#include "functions/overrides/process-env.h"
#include "functions/overrides/process-threads-api.h"
#include "functions/overrides/undocumented-api.h"
#include "functions/overrides/win-base.h"

#include "attach-detours.h"

using namespace Monitor::Windows;

bool APIENTRY DllMain(HINSTANCE hModule, DWORD dwReason, PVOID lpReserved)
{
	(void)hModule;
	(void)lpReserved;

#ifdef ENABLE_MONITOR_DEBUG
	while (!IsDebuggerPresent())
	{
		Sleep(0);
	}
#endif

	if (DetourIsHelperProcess())
	{
		return true;
	}

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DetourRestoreAfterWith();
		Functions::Cache::UndocumentedApi::EntryPoint = (int (WINAPI *)(void))DetourGetEntryPoint(nullptr);
		return ProcessAttach(hModule);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		return ProcessDetach(hModule);
	}
	else if (dwReason == DLL_THREAD_ATTACH)
	{
		return ThreadAttach(hModule);
	}
	else if (dwReason == DLL_THREAD_DETACH)
	{
		return ThreadDetach(hModule);
	}

	return true;
}