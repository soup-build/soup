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
	private:
		AppState _state;

	public:
		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		void Run(Core::PackageProvider& packageProvider)
		{
			_state = AppState();
			InitializeState(packageProvider);

			auto app = ftxui::App::Fullscreen();

			auto packagesMenu = CreateSingleItemMenu(&_state.PackagesList, &_state.PackagesListSelected);

			auto tabComponents = CreateAllPackageTabs(packageProvider);

			auto packagesPropertiesView = ftxui::Container::Tab(
				std::move(tabComponents),
				&_state.PackagesListSelected);

			auto rendererProperties = ftxui::Renderer(packagesPropertiesView, [&] {
				return packagesPropertiesView->Render() | ftxui::vscroll_indicator | ftxui::xflex_grow | ftxui::frame;
			});

			auto packagesView = ftxui::Container::Horizontal({
				packagesMenu,
				rendererProperties,
			});

			auto packagesViewRenderer = ftxui::Renderer(packagesView, [&] {
				return ftxui::vbox({
					AppAsciiArt(),
					ftxui::hbox({
						packagesMenu->Render(),
						ftxui::separator(),
						rendererProperties->Render(),
					}) |
					ftxui::border,
				});
			});

			app.Loop(packagesViewRenderer);
		}

	private:
		void InitializeState(Core::PackageProvider& packageProvider)
		{
			for (auto& [key, value] : packageProvider.GetPackageLookup())
			{
				_state.PackagesList.push_back(value.Name.ToString());
			}

			_state.PackagesListSelected = 0;

			_state.PackageTabList = std::vector<std::string>({
				"Properties",
				"Tasks",
				"Operations",
			});
			_state.PackageTabSelected = 0;
		}

		ftxui::Components CreateAllPackageTabs(Core::PackageProvider& packageProvider)
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

				auto tab_toggle = ftxui::Toggle(&_state.PackageTabList, &_state.PackageTabSelected);

				auto propertiesList = ftxui::Container::Vertical(std::move(properties));
				auto tasksList = ftxui::Container::Vertical({
					CreateSingleItemMenuEntry("Tasks"),
				});
				auto operationsList = ftxui::Container::Vertical({
					CreateSingleItemMenuEntry("Operations"),
				});

				auto tab_container = ftxui::Container::Tab({
						propertiesList,
						tasksList,
						operationsList,
					},
					&_state.PackageTabSelected);

				auto container = ftxui::Container::Vertical({
					tab_toggle,
					tab_container,
				});

				auto renderer = ftxui::Renderer(container, [tab_toggle, tab_container] {
					return ftxui::vbox({
							tab_toggle->Render(),
							ftxui::separator(),
							tab_container->Render(),
						}) |
						ftxui::border;
				});

				tabComponents.push_back(renderer);
			}

			return tabComponents;
		}
	};
}
