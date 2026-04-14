// <copyright file="view-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "view-options.h"

namespace Soup::Client
{
	using namespace ftxui;

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
	/// View Command
	/// </summary>
	class ViewCommand : public ICommand
	{
	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="ViewCommand"/> class.
		/// </summary>
		ViewCommand(ViewOptions options) :
			_options(std::move(options))
		{
		}

		/// <summary>
		/// Main entry point for a unique command
		/// </summary>
		virtual void Run() override final
		{
			Log::Diag("ViewsCommand::Run");

			auto workingDirectory = Path();
			if (_options.Path.empty())
			{
				// Build in the current directory
				workingDirectory = System::IFileSystem::Current().GetCurrentDirectory();
			}
			else
			{
				// Parse the path in any system valid format
				workingDirectory = Path::Parse(std::format("{}/", _options.Path));

				// Check if this is relative to current directory
				if (!workingDirectory.HasRoot())
				{
					workingDirectory = System::IFileSystem::Current().GetCurrentDirectory() + workingDirectory;
				}
			}

			// Platform specific defaults
			#if defined(_WIN32)
				auto hostPlatform = "Windows";
			#elif defined(__linux__)
				auto hostPlatform = "Linux";
			#else
				#error "Unknown Platform"
			#endif

			// Load user config state
			auto userDataPath = Core::Build::Constants::GetSoupUserDataPath();
			
			auto recipeCache = Core::RecipeCache();

			// Setup the build parameters
			auto globalParameters = Core::ValueTable();

			// Process well known parameters
			if (!_options.Flavor.empty())
				globalParameters.emplace("Flavor", Core::Value(_options.Flavor));
			if (!_options.Architecture.empty())
				globalParameters.emplace("Architecture", Core::Value(_options.Architecture));

			auto packageProvider = Core::Build::LoadBuildGraph(
				workingDirectory,
				_options.Owner,
				globalParameters,
				userDataPath,
				hostPlatform,
				recipeCache);

			auto state = AppState();
			InitializeState(state, packageProvider);

			auto app = App::Fullscreen();

			auto tab_menu = SingleItemMenu(&state.PackagesList, &state.PackagesListSelected);
			auto rendererMenu = Renderer(tab_menu, [&] {
				return tab_menu->Render() | vscroll_indicator | frame;
			});

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

			auto packagesPropertiesView = Container::Tab(
				std::move(tabComponents),
				&state.PackagesListSelected);

			auto container2 = Container::Horizontal({
				rendererMenu,
				packagesPropertiesView,
			});

			auto packagesView = Renderer(container2, [&] {
				return hbox({
					rendererMenu->Render(),
					separator(),
					packagesPropertiesView->Render(),
				}) |
				border;
			});

			app.Loop(packagesView);
		}

	private:
		void InitializeState(AppState& state, Core::PackageProvider& packageProvider)
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

		Component RendererInfo(const std::string& name, int* size)
		{
			return Renderer([name, size] {
				return text(name + ": " + std::to_string(*size)) | center;
			});
		}

		// Take a list of component, display them vertically, one column shifted to the
		// right.
		Component Inner(std::vector<Component> children) {
			auto vlist = Container::Vertical(std::move(children));
			return Renderer(vlist, [vlist] {
				return hbox({
					text(" "),
					vlist->Render(),
				});
			});
		}

		Component Empty()
		{
			return Inner({
				CreateSingleItemMenuEntry("EMPTY"),
				CreateSingleItemMenuEntry("EMPTY"),
			});
		}

		Component CollapsableItems()
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

		Component CreateSingleItemMenuEntry(std::string_view value)
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

		Component SingleItemMenu(std::vector<std::string>* entries, int* selected) {
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

	private:
		ViewOptions _options;
	};
}
