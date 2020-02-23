﻿// <copyright file="STLFileSystem.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "IFileSystem.h"

namespace Opal::System
{
	/// <summary>
	/// The standard library file system
	/// </summary>
	export class STLFileSystem : public IFileSystem
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref='STLFileSystem'/> class.
		/// </summary>
		STLFileSystem()
		{
		}

		/// <summary>
		/// Gets the current directory for the running processes
		/// </summary>
		Path GetCurrentDirectory2() override final
		{
			auto current = std::filesystem::current_path();
			return Path(current.string());
		}

		/// <summary>
		/// Gets a value indicating whether the directory/file exists
		/// </summary>
		bool Exists(const Path& path) override final
		{
			return std::filesystem::exists(path.ToString());
		}

		/// <summary>
		/// Get the last write time of the file/directory
		/// </summary>
		std::time_t GetLastWriteTime(const Path& path) override final
		{
			#if defined ( _WIN32 )
				struct _stat64 fileInfo;
				if (_stat64(path.ToString().c_str(), &fileInfo) != 0)
					throw std::runtime_error("Failed to get last write time.");
				return fileInfo.st_mtime;
			#else
				auto fileTime = std::filesystem::last_write_time(path.ToString());
				auto time = decltype(fileTime)::clock::to_time_t(fileTime);
				return time;
			#endif
		}

		/// <summary>
		/// Open the requested file as a stream to read
		/// </summary>
		std::shared_ptr<std::istream> OpenRead(const Path& path, bool isBinary) override final
		{
			int mode = std::fstream::in;
			if (isBinary)
			{
				mode = mode | std::fstream::binary;
			}

			auto file = std::make_shared<std::fstream>(path.ToString(), mode);
			if (file->fail())
			{
				auto message = "OpenRead Failed: File missing. " + path.ToString();
				throw std::runtime_error(std::move(message));
			}

			return file;
		}

		/// <summary>
		/// Open the requested file as a stream to write
		/// </summary>
		std::shared_ptr<std::ostream> OpenWrite(const Path& path) override final
		{
			auto file = std::make_shared<std::fstream>(path.ToString(), std::fstream::out);
			return file;
		}
		
		/// <summary>
		/// Copy the source file to the destination
		/// </summary>
		void CopyFile2(const Path& source, const Path& destination) override final
		{
			std::filesystem::copy(
				source.ToString(),
				destination.ToString(),
				std::filesystem::copy_options::overwrite_existing);
		}

		/// <summary>
		/// Create the directory at the requested path
		/// </summary>
		void CreateDirectory2(const Path& path) override final
		{
			std::filesystem::create_directories(path.ToString());
		}

		/// <summary>
		/// Get the children of a directory
		/// </summary>
		std::vector<DirectoryEntry> GetDirectoryChildren(const Path& path) override final
		{
			auto result = std::vector<DirectoryEntry>();
			for(auto& child : std::filesystem::directory_iterator(path.ToString()))
			{
				result.push_back(
					{
						Path(child.path().string()),
						child.is_directory()
					});
			}

			return result;
		}

		/// <summary>
		/// Delete the directory
		/// </summary>
		void DeleteDirectory(const Path& path, bool recursive) override final
		{
			if (recursive)
			{
				std::filesystem::remove_all(path.ToString());
			}
			else
			{
				std::filesystem::remove(path.ToString());
			}
		}
	};
}
