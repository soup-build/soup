// <copyright file="format-utils.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <chrono>
#include <string>

export module Soup.Core:FormatUtils;

namespace Soup::Core {
	/// <summary>
	/// A utility class for formatting string values
	/// </summary>
	export class FormatUtils {
	public:
		static std::string FormatBool(bool value) {
			return value ? "true" : "false";
		}
		
		static std::string FormatTime(std::chrono::time_point<std::chrono::file_clock> time) {
			auto sys_time =
				std::chrono::clock_cast<std::chrono::system_clock>(time);
			std::chrono::zoned_time local_time{std::chrono::current_zone(), sys_time};
			return std::format("{:%Y-%m-%d %H:%M:%S}", local_time);
		}
	};
}
