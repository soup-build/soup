// <copyright file="custom-style.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <string>
#include <vector>

export module Soup.View:CustomStyle;

import ftxui;

namespace Soup::View
{
	export ftxui::Component AppAsciiArt(bool* showAsciiArt)
	{
		auto asciiArt = ftxui::Renderer([] {
			return ftxui::vbox({
				ftxui::text(R"(  _________                     __________       __ __        ___)"),
				ftxui::text(R"( /   _____/ ____  __ ________   \______   \__ __|__|  |    __| _/)"),
				ftxui::text(R"( \_____  \ /  _ \|  |  \____ \   |    |  _/  |  \  |  |   / __ | )"),
				ftxui::text(R"( /        (  (_) )  |  /  |_) )  |    |   \  |  /  |  |__/ /_/ | )"),
				ftxui::text(R"(/_______  /\____/|____/|   __/   |______  /____/|__|____/\____ | )"),
				ftxui::text(R"(        \/             |__|             \/                    \/ )"),
			}) | ftxui::color(ftxui::Color::HotPink);
		});

		// Only show the art if there is plenty of room
		auto conditionalView = ftxui::Maybe(asciiArt, showAsciiArt);
		return conditionalView;
	}

	export ftxui::Component CustomToggle(std::vector<std::string>&& entries, ftxui::Ref<int> selected)
	{
		auto option = ftxui::MenuOption::Toggle();
		option.entries_option.transform = [](const ftxui::EntryState& state) {
			auto element = ftxui::text(state.label);
			if (state.focused) {
				element |= ftxui::inverted;
			}
			if (state.active) {
				element |= ftxui::bold;
				element |= ftxui::color(ftxui::Color::HotPink);
			}
			if (!state.focused && !state.active) {
				element |= ftxui::dim;
			}

			return element;
		};

		option.entries = std::move(entries);
		option.selected = selected;

		return ftxui::Menu(option);
	}

	// Take a list of component, display them vertically, one column shifted to the right.
	export ftxui::Component Inner(std::vector<ftxui::Component> children)
	{
		auto vlist = ftxui::Container::Vertical(std::move(children));
		return ftxui::Renderer(vlist, [vlist] {
			return ftxui::hbox({
				ftxui::text(" "),
				vlist->Render(),
			});
		});
	}

	export ftxui::Component CreateSingleItemMenuEntry(std::string_view value)
	{
		auto option = ftxui::MenuEntryOption();
		option.transform = [](ftxui::EntryState state) {
			auto prefix = ftxui::text("  ");
			auto label = ftxui::text(state.label);
			auto element = ftxui::hbox({prefix, label});
			if (state.focused) {
				element |= ftxui::color(ftxui::Color::HotPink);
			}
			if (state.active) {
				element |= ftxui::bold;
			}
			return element;
		};

		return MenuEntry(value, option);
	}

	export ftxui::Component CreateSingleItemMenu(std::vector<std::string> entries, ftxui::Ref<int> selected) {
		auto option = ftxui::MenuOption::Vertical();
		option.entries_option.transform = [](ftxui::EntryState state) {
			auto prefix = ftxui::text(state.active ? "│ " : "  ");
			auto label = ftxui::text(state.label);
			auto element = ftxui::hbox({prefix, label});
			if (state.focused) {
				element |= ftxui::inverted;
			}
			if (state.active) {
				element |= ftxui::color(ftxui::Color::HotPink);
			}

			return element;
		};

		option.entries = std::move(entries);
		option.selected = selected;
		
		auto menu = ftxui::Menu(option);

		// Wrap the menu in a renderer to add a frame and scroll indicator
		auto rendererMenu = ftxui::Renderer(menu, [menu] {
			return menu->Render() | ftxui::vscroll_indicator | ftxui::frame;
		});

		return rendererMenu;
	}
}
