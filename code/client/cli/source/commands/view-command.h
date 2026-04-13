// <copyright file="view-command.h" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

#pragma once
#include "i-command.h"
#include "view-options.h"

namespace Soup::Client
{
	using namespace ftxui;

	struct PackageState
	{
		std::vector<std::string> PropertiesList;
		int PropertiesListSelected;
	};

	struct AppState
	{
		int left_size;
		int right_size;
		int top_size;
		int bottom_size;

		std::vector<std::string> PackagesList;
		std::vector<PackageState> PackagePropertiesList;
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

			auto tab_menu = VMenu1(&state.PackagesList, &state.PackagesListSelected);
			auto rendererMenu = Renderer(tab_menu, [&] {
				return tab_menu->Render() | vscroll_indicator | frame;
			});

			auto tabComponents = Components();
			for (auto& packageProperties : state.PackagePropertiesList)
			{
				tabComponents.push_back(
					VMenu1(&packageProperties.PropertiesList, &packageProperties.PropertiesListSelected));
			}

			auto tab_container = Container::Tab(
				std::move(tabComponents),
				&state.PackagesListSelected);

			auto container2 = Container::Horizontal({
				rendererMenu,
				tab_container,
			});

			auto renderer2 = Renderer(container2, [&] {
				return hbox({
					rendererMenu->Render(),
					separator(),
					tab_container->Render(),
				}) |
				border;
			});

			app.Loop(renderer2);
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

				auto properties = std::vector<std::string>();
				properties.push_back(std::to_string(value.Id));
				properties.push_back(value.Name.ToString());
				properties.push_back(value.PackageRoot.ToString());

				for (auto& [dependency, dependencyInfo] : value.Dependencies)
				{
					properties.push_back(dependency);
				}

				state.PackagePropertiesList.push_back({
					std::move(properties),
					0,
				});
			}

			state.PackagesListSelected = 0;
		}

		Component RendererInfo(const std::string& name, int* size)
		{
			return Renderer([name, size] {
				return text(name + ": " + std::to_string(*size)) | center;
			});
		}

		Component VMenu1(std::vector<std::string>* entries, int* selected) {
			auto option = MenuOption::Vertical();
			option.entries_option.transform = [](EntryState state) {
				state.label = (state.active ? "| " : "  ") + state.label;
				Element e = text(state.label);
				if (state.focused) {
					e = e | color(Color::HotPink);
				}
				if (state.active) {
					e = e | bold;
				}
				return e;
			};
			return Menu(entries, selected, option);
		}

	private:
		ViewOptions _options;
	};
}
