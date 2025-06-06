#pragma once
#include "../Cache/ProcessThreadsApi.h"
#include "WindowsHelpers.h"

namespace Monitor::Windows::Functions::Overrides::ProcessThreadsApi
{
	BOOL WINAPI CreateProcessA(
		LPCSTR lpApplicationName,
		LPSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation)
	{
		auto message = MessageSender(MessageType::Detour);
		message.AppendValue(static_cast<uint32_t>(DetourEventType::CreateProcessA));

		BOOL result = 0;
		bool wasDetoured = false;
		
		// TODO: Get name from args if not passed in
		auto applicationName = std::string_view();
		if (lpApplicationName != nullptr)
			applicationName = lpApplicationName;

		if (IsWhiteListProcess(applicationName))
		{
			result = Cache::ProcessThreadsApi::CreateProcessA(
				lpApplicationName,
				lpCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags,
				lpEnvironment,
				lpCurrentDirectory,
				lpStartupInfo,
				lpProcessInformation);
		}
		else
		{
			wasDetoured = true;

			// If the caller did not request the process information then create a temporary
			// one for ourselves
			LPPROCESS_INFORMATION lpInternalProcessInformation = lpProcessInformation;
			PROCESS_INFORMATION privateProcessInformation;
			if (lpInternalProcessInformation == nullptr)
			{
				lpInternalProcessInformation = &privateProcessInformation;
			}

			// Pass along the request with an initially suspended process
			result = DetourCreateProcessWithDllExA(
				lpApplicationName,
				lpCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags | CREATE_SUSPENDED,
				lpEnvironment,
				lpCurrentDirectory,
				lpStartupInfo,
				lpInternalProcessInformation,
				s_szDllPath,
				Cache::ProcessThreadsApi::CreateProcessA);

			if (result)
			{
				// Perform the detour setup
				CreateProcessInternals(lpInternalProcessInformation->hProcess);

				// If the caller did not create the process suspended then undo our override
				if (!(dwCreationFlags & CREATE_SUSPENDED))
				{
					ResumeThread(lpInternalProcessInformation->hThread);
				}

				// Cleanup if we used the private information store
				if (lpInternalProcessInformation == &privateProcessInformation)
				{
					CloseHandle(privateProcessInformation.hThread);
					CloseHandle(privateProcessInformation.hProcess);
				}
			}
		}

		message.AppendValue(wasDetoured);
		message.AppendValue(lpApplicationName);
		message.AppendValue(result);

		return result;
	}

	BOOL WINAPI CreateProcessW(
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation)
	{
		auto message = MessageSender(MessageType::Detour);
		message.AppendValue(static_cast<uint32_t>(DetourEventType::CreateProcessW));

		BOOL result = 0;
		bool wasDetoured = false;

		// TODO: Get name from args if not passed in
		auto applicationName = std::wstring_view();
		if (lpApplicationName != nullptr)
			applicationName = lpApplicationName;

		if ( IsWhiteListProcess(applicationName))
		{
			result = Cache::ProcessThreadsApi::CreateProcessW(
				lpApplicationName,
				lpCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags,
				lpEnvironment,
				lpCurrentDirectory,
				lpStartupInfo,
				lpProcessInformation);
		}
		else
		{
			wasDetoured = true;

			// If the caller did not request the process information then create a temporary
			// one for ourselves
			LPPROCESS_INFORMATION lpInternalProcessInformation = lpProcessInformation;
			PROCESS_INFORMATION privateProcessInformation;
			if (lpInternalProcessInformation == nullptr)
			{
				lpInternalProcessInformation = &privateProcessInformation;
			}

			// Pass along the request with an initially suspended process
			result = DetourCreateProcessWithDllExW(
				lpApplicationName,
				lpCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags | CREATE_SUSPENDED,
				lpEnvironment,
				lpCurrentDirectory,
				lpStartupInfo,
				lpInternalProcessInformation,
				s_szDllPath,
				Cache::ProcessThreadsApi::CreateProcessW);

			if (result)
			{
				// Perform the detour setup
				CreateProcessInternals(lpInternalProcessInformation->hProcess);

				// If the caller did not create the process suspended then undo our override
				if (!(dwCreationFlags & CREATE_SUSPENDED))
				{
					ResumeThread(lpInternalProcessInformation->hThread);
				}

				// Cleanup if we used the private information store
				if (lpInternalProcessInformation == &privateProcessInformation)
				{
					CloseHandle(privateProcessInformation.hThread);
					CloseHandle(privateProcessInformation.hProcess);
				}
			}
		}

		message.AppendValue(wasDetoured);
		message.AppendValue(lpApplicationName);
		message.AppendValue(result);

		return result;
	}

	BOOL WINAPI CreateProcessAsUserA(
		HANDLE hToken,
		LPCSTR lpApplicationName,
		LPSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCSTR lpCurrentDirectory,
		LPSTARTUPINFOA lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation)
	{
		auto message = MessageSender(MessageType::Detour);
		message.AppendValue(static_cast<uint32_t>(DetourEventType::CreateProcessAsUserA));

		BOOL result = Cache::ProcessThreadsApi::CreateProcessAsUserA(
			hToken,
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation);

		message.AppendValue(lpApplicationName);
		message.AppendValue(result);

		// TODO
		{
			auto errorMessage = MessageSender(MessageType::Error);
			errorMessage.AppendValue("Cannot detour process from CreateProcessAsUserA");
		}

		return result;
	}

	BOOL WINAPI CreateProcessAsUserW(
		HANDLE hToken,
		LPCWSTR lpApplicationName,
		LPWSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCWSTR lpCurrentDirectory,
		LPSTARTUPINFOW lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation)
	{
		auto message = MessageSender(MessageType::Detour);
		message.AppendValue(static_cast<uint32_t>(DetourEventType::CreateProcessAsUserW));

		BOOL result = Cache::ProcessThreadsApi::CreateProcessAsUserW(
			hToken,
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation);

		message.AppendValue(lpApplicationName);
		message.AppendValue(result);

		// TODO
		{
			auto errorMessage = MessageSender(MessageType::Error);
			errorMessage.AppendValue("Cannot detour process from CreateProcessAsUserW");
		}

		return result;
	}

	void WINAPI ExitProcess(UINT uExitCode)
	{
		auto message = MessageSender(MessageType::Detour);
		message.AppendValue(static_cast<uint32_t>(DetourEventType::ExitProcess));

		Cache::ProcessThreadsApi::ExitProcess(uExitCode);

		message.AppendValue(uExitCode);
	}
}