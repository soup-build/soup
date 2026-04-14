// <copyright file="view-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
using namespace ftxui;

namespace Soup::View
{
	// Create custom collapsible so we can style it
	// TODO: Make collapsible extensible with options
	Component CustomCollapsible(ConstStringRef label, Component child, Ref<bool> show = false)
	{
		class Impl : public ComponentBase {
		public:
			Impl(ConstStringRef label, Component child, Ref<bool> show) : show_(show)
			{
				CheckboxOption option;
				option.transform = [](EntryState state)
				{
					auto prefix = text(state.state ? "  ▼ " : "  ▶ ");
					auto label = text(state.label);
					auto element = hbox({prefix, label});
					if (state.active) {
						element |= bold;
					}
					if (state.focused) {
						element |= color(Color::HotPink);
					}
					return element;	
				};
				Add(Container::Vertical({
					Checkbox(std::move(label), show_.operator->(), option),
					Maybe(std::move(child), show_.operator->()),
				}));
			}

			Ref<bool> show_;
		};

		return Make<Impl>(std::move(label), std::move(child), show);
	}

	struct AppState
	{
		int left_size;
		int right_size;
		int top_size;
		int bottom_size;

		std::vector<std::string> PackagesList;
		int PackagesListSelected;
	};

	/// <summary>
	/// TUI
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

			auto app = App::Fullscreen();

			auto tab_menu = SingleItemMenu(&state.PackagesList, &state.PackagesListSelected);
			auto rendererMenu = Renderer(tab_menu, [&] {
				return tab_menu->Render() | vscroll_indicator | frame;
			});

			auto tabComponents = CreateAllPackageTabs(packageProvider);
			

			auto packagesPropertiesView = Container::Tab(
				std::move(tabComponents),
				&state.PackagesListSelected);

			auto rendererProperties = Renderer(packagesPropertiesView, [&] {
				return packagesPropertiesView->Render() | vscroll_indicator | xflex_grow | frame;
			});

			auto container2 = Container::Horizontal({
				rendererMenu,
				rendererProperties,
			});

			auto packagesView = Renderer(container2, [&] {
				return vbox({
					text(R"(  _________                     __________      .__.__       .___)") | color(Color::HotPink),
					text(R"( /   _____/ ____  __ ________   \______   \__ __|__|  |    __| _/)") | color(Color::HotPink),
					text(R"( \_____  \ /  _ \|  |  \____ \   |    |  _/  |  \  |  |   / __ | )") | color(Color::HotPink),
					text(R"( /        (  <_> )  |  /  |_> >  |    |   \  |  /  |  |__/ /_/ | )") | color(Color::HotPink),
					text(R"(/_______  /\____/|____/|   __/   |______  /____/|__|____/\____ | )") | color(Color::HotPink),
					text(R"(        \/             |__|             \/                    \/ )") | color(Color::HotPink),
					hbox({
						rendererMenu->Render(),
						separator(),
						rendererProperties->Render(),
					}) |
					border,
				});
			});

			app.Loop(packagesView);
		}

	private:
		static void InitializeState(AppState& state, Core::PackageProvider& packageProvider)
		{
			state.left_size = 20;
			state.right_size = 20;
			state.top_size = 10;
			state.bottom_size = 10;

			for (auto& [key, value] : packageProvider.GetPackageLookup())
			{
				state.PackagesList.push_back(value.Name.ToString());
			}

			state.PackagesListSelected = 0;
		}

		static Components CreateAllPackageTabs(Core::PackageProvider& packageProvider)
		{
			auto tabComponents = Components();
			for (auto& [key, value] : packageProvider.GetPackageLookup())
			{
				auto properties = Components();

				properties.push_back(CreateSingleItemMenuEntry(std::to_string(value.Id)));
				properties.push_back(CreateSingleItemMenuEntry(value.Name.ToString()));
				properties.push_back(CreateSingleItemMenuEntry(value.PackageRoot.ToString()));

				for (auto& [dependencyType, dependencies] : value.Dependencies)
				{
					auto dependencyItems = Components();
					for (auto& dependency : dependencies)
					{
						dependencyItems.push_back(CreateSingleItemMenuEntry(dependency.OriginalReference.ToString()));
					}

					properties.push_back(Collapsible(dependencyType, Inner(dependencyItems)));
				}

				tabComponents.push_back(
					Container::Vertical(std::move(properties)));
			}

			return tabComponents;
		}

		static Component RendererInfo(const std::string& name, int* size)
		{
			return Renderer([name, size] {
				return text(name + ": " + std::to_string(*size)) | center;
			});
		}

		// Take a list of component, display them vertically, one column shifted to the
		// right.
		static Component Inner(std::vector<Component> children)
		{
			auto vlist = Container::Vertical(std::move(children));
			return Renderer(vlist, [vlist] {
				return hbox({
					text(" "),
					vlist->Render(),
				});
			});
		}

		static Component Empty()
		{
			return Inner({
				CreateSingleItemMenuEntry("EMPTY"),
				CreateSingleItemMenuEntry("EMPTY"),
			});
		}

		static Component CollapsableItems()
		{
			auto component = Collapsible("Collapsible 1",
				Inner({
					Collapsible(
						"Collapsible 1.1",
						Inner({
							Collapsible("Collapsible 1.1.1", Empty()),
							Collapsible("Collapsible 1.1.2", Empty()),
							Collapsible("Collapsible 1.1.3", Empty()),
						})),
					Collapsible(
						"Collapsible 1.2",
						Inner({
							Collapsible("Collapsible 1.2.1", Empty()),
							Collapsible("Collapsible 1.2.2", Empty()),
							Collapsible("Collapsible 1.2.3", Empty()),
						})),
					Collapsible(
						"Collapsible 1.3",
						Inner({
							Collapsible("Collapsible 1.3.1", Empty()),
							Collapsible("Collapsible 1.3.2", Empty()),
							Collapsible("Collapsible 1.3.3", Empty()),
						})),
				}));

			return component;
		}

		static Component CreateSingleItemMenuEntry(std::string_view value)
		{
			auto option = MenuEntryOption();
			option.transform = [](EntryState state) {
				auto prefix = text("  ");
				auto label = text(state.label);
				auto element = hbox({prefix, label});
				if (state.focused) {
					element |= color(Color::HotPink);
				}
				if (state.active) {
					element |= bold;
				}
				return element;
			};

			return MenuEntry(value, option);
		}

		static Component SingleItemMenu(std::vector<std::string>* entries, int* selected) {
			auto option = MenuOption::Vertical();
			option.entries_option.transform = [](EntryState state) {
				auto prefix = text(state.active ? "│ " : "  ");
				auto label = text(state.label);
				auto element = hbox({prefix, label});
				if (state.focused) {
					element |= bold;
				}
				if (state.active) {
					element |= color(Color::HotPink);
				}

				return element;
			};

			return Menu(entries, selected, option);
		}
	};
}
