
#pragma once
#include "connection-manager.h"
#include "helpers.h"

namespace Monitor {
	class FileSystemAccessSandbox {
	public:
		static void Initialize(
			bool enableAccessChecks,
			Opal::Path&& workingDirectory,
			std::vector<Opal::Path>&& allowedReadDirectories,
			std::vector<Opal::Path>&& allowedWriteDirectories) {
			connectionManager.DebugTrace("FileSystemAccessSandbox::Initialize");

			m_enableAccessChecks = enableAccessChecks;
			m_workingDirectory = std::move(workingDirectory);
			m_allowedReadDirectories = std::move(allowedReadDirectories);
			m_allowedWriteDirectories = std::move(allowedWriteDirectories);
		}

		static void UpdateWorkingDirectory(const wchar_t *fileName) {
			auto fileNameEncoded = UTF8Encode(fileName);
			return UpdateWorkingDirectory(fileNameEncoded.c_str());
		}

		static void UpdateWorkingDirectory(const char *fileName) {
			auto path = Opal::Path::ParseWindows(fileName);
			if (!path.HasRoot()) {
				// Updated working directory is relative to the previous one
				m_workingDirectory = m_workingDirectory + path;
			} else {
				m_workingDirectory = std::move(path);
			}
		}

		static bool IsAllowed(const wchar_t *fileName, DWORD desiredAccess) {
			bool isWrite = (desiredAccess & GENERIC_WRITE) != 0;
			if (isWrite)
				return IsWriteAllowed(fileName);
			else
				return IsReadAllowed(fileName);
		}

		static bool IsAllowed(const char *fileName, DWORD desiredAccess) {
			bool isWrite = (desiredAccess & GENERIC_WRITE) != 0;
			if (isWrite)
				return IsWriteAllowed(fileName);
			else
				return IsReadAllowed(fileName);
		}

		static bool IsReadAllowed(const char *fileName) {
			if (!m_enableAccessChecks)
				return true;

			if (IsPipe(fileName))
				return true;

			auto normalizedFileName = NormalizePath(fileName);
			for (const auto &allowedDirectory : m_allowedReadDirectories) {
				if (IsUnderDirectory(normalizedFileName, allowedDirectory)) {
					return true;
				}
			}

			ConnectionManagerBase::DebugTrace("Block: {} {}", fileName, normalizedFileName.ToString());
			return false;
		}

		static bool IsReadAllowed(const wchar_t *fileName) {
			if (!m_enableAccessChecks)
				return true;

			auto fileNameEncoded = UTF8Encode(fileName);
			return IsReadAllowed(fileNameEncoded.c_str());
		}

		static bool IsWriteAllowed(const char *fileName) {
			if (!m_enableAccessChecks)
				return true;

			if (IsPipe(fileName))
				return true;

			auto normalizedFileName = NormalizePath(fileName);
			for (const auto &allowedDirectory : m_allowedWriteDirectories) {
				if (IsUnderDirectory(normalizedFileName, allowedDirectory))
					return true;
			}

			return false;
		}

		static bool IsWriteAllowed(const wchar_t *fileName) {
			if (!m_enableAccessChecks)
				return true;

			auto fileNameEncoded = UTF8Encode(fileName);
			return IsWriteAllowed(fileNameEncoded.c_str());
		}

	private:
		static bool IsPipe(const char *fileName) {
			auto file = std::string(fileName);
			return file.starts_with("\\\\.\\");
		}

		static Opal::Path NormalizePath(const char *fileName) {
			// Normalize the path separators and get absolute path
			auto path = Opal::Path::ParseWindows(fileName);
			if (!path.HasRoot()) {
				path = m_workingDirectory + path;
			}

			return path;
		}

		static bool IsUnderDirectory(const Opal::Path &fileName, const Opal::Path &directory) {
			return StartsWithIgnoreCase(fileName.ToString(), directory.ToString());
		}

		static bool StartsWithIgnoreCase(std::string_view lhs, std::string_view rhs) {
			// A string cannot start with a prefix longer than itself
			if (lhs.length() < rhs.length()) {
				return false;
			}

			// Compare characters up to the length of the prefix
			return std::equal(rhs.begin(), rhs.end(), lhs.begin(), 
				[](unsigned char a, unsigned char b) {
					return std::toupper(a) == std::toupper(b);
				});
		}

		static std::string UTF8Encode(const std::wstring_view wideString) {
			if (wideString.empty())
				return std::string();

			int requiredSizes = WideCharToMultiByte(
				CP_UTF8,
				0,
				wideString.data(),
				(int)wideString.size(),
				nullptr,
				0,
				nullptr,
				nullptr);

			std::string result(requiredSizes, 0);
			WideCharToMultiByte(
				CP_UTF8,
				0,
				wideString.data(),
				(int)wideString.size(),
				result.data(),
				requiredSizes,
				nullptr,
				nullptr);

			return result;
		}

		static bool m_enableAccessChecks;
		static Opal::Path m_workingDirectory;

		static std::vector<Opal::Path> m_allowedReadDirectories;
		static std::vector<Opal::Path> m_allowedWriteDirectories;
	};

	bool FileSystemAccessSandbox::m_enableAccessChecks = false;
	Opal::Path FileSystemAccessSandbox::m_workingDirectory = Opal::Path();
	std::vector<Opal::Path> FileSystemAccessSandbox::m_allowedReadDirectories =
		std::vector<Opal::Path>();
	std::vector<Opal::Path> FileSystemAccessSandbox::m_allowedWriteDirectories =
		std::vector<Opal::Path>();
}
