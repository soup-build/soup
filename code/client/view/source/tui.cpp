// <copyright file="tui.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <functional>
#include <memory>
#include <vector>

export module Soup.View:TUI;

import ftxui;
import Soup.Core;
import :AppState;
import :CustomStyle;

namespace Soup::View
{
	/// <summary>
	/// Text/Terminal UI
	/// </summary>
	export class TUI
	{
	public:
		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		static void Run(Core::PackageProvider& packageProvider)
		{
			auto state = AppState();
			InitializeState(state, packageProvider);

			auto app = ftxui::App::Fullscreen();

			auto tab_menu = SingleItemMenu(&state.PackagesList, &state.PackagesListSelected);
			auto rendererMenu = ftxui::Renderer(tab_menu, [&] {
				return tab_menu->Render() | ftxui::vscroll_indicator | ftxui::frame;
			});

			auto tabComponents = CreateAllPackageTabs(packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&state.PackagesListSelected);

			auto rendererProperties = ftxui::Renderer(packagesPropertiesView, [&] {
				return packagesPropertiesView->Render() | ftxui::vscroll_indicator | ftxui::xflex_grow | ftxui::frame;
			});

			auto container2 = ftxui::Container::Horizontal({
				rendererMenu,
				rendererProperties,
			});

			auto packagesView = ftxui::Renderer(container2, [&] {
				return ftxui::vbox({
					AppAsciiArt(),
					ftxui::hbox({
						rendererMenu->Render(),
						ftxui::separator(),
						rendererProperties->Render(),
					}) |
					ftxui::border,
				});
			});

			app.Loop(packagesView);
		}

	private:
		static void InitializeState(AppState& state, Core::PackageProvider& packageProvider)
		{
			for (auto& [key, value] : packageProvider.GetPackageLookup())
			{
				state.PackagesList.push_back(value.Name.ToString());
			}

			state.PackagesListSelected = 0;
		}

		static ftxui::Components CreateAllPackageTabs(Core::PackageProvider& packageProvider)
		{
			auto tabComponents = ftxui::Components();
			for (auto& [key, value] : packageProvider.GetPackageLookup())
			{
				auto properties = ftxui::Components();

				properties.push_back(CreateSingleItemMenuEntry(std::to_string(value.Id)));
				properties.push_back(CreateSingleItemMenuEntry(value.Name.ToString()));
				properties.push_back(CreateSingleItemMenuEntry(value.PackageRoot.ToString()));

				for (auto& [dependencyType, dependencies] : value.Dependencies)
				{
					auto dependencyItems = ftxui::Components();
					for (auto& dependency : dependencies)
					{
						dependencyItems.push_back(CreateSingleItemMenuEntry(dependency.OriginalReference.ToString()));
					}

					properties.push_back(ftxui::Collapsible(dependencyType, Inner(dependencyItems)));
				}

				tabComponents.push_back(
					ftxui::Container::Vertical(std::move(properties)));
			}

			return tabComponents;
		}
	};
}
