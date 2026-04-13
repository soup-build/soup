#include <memory>
#include <optional>
#include <string>
#include <vector>

import ftxui;
import Opal;
import Soup.Core;

using namespace ftxui;
using namespace Opal;
using namespace Soup::Core;

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

void InitializeState(AppState& state, PackageProvider& packageProvider)
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

PackageProvider LoadGraph(const Path& workingDirectory, const ValueTable& globalParameters)
{
	// Platform specific defaults
	#if defined(_WIN32)
		auto hostPlatform = "Windows";
	#elif defined(__linux__)
		auto hostPlatform = "Linux";
	#else
		#error "Unknown Platform"
	#endif

	// Load user config state
	auto userDataPath = Build::Constants::GetSoupUserDataPath();
	
	auto recipeCache = RecipeCache();

	auto packageProvider = Build::LoadBuildGraph(
		workingDirectory,
		std::nullopt,
		globalParameters,
		userDataPath,
		hostPlatform,
		recipeCache);

	return packageProvider;
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


int main()
{
	// Setup the filter
	auto defaultTypes =
		// static_cast<uint32_t>(TraceEventFlag::Diagnostic) |
		// static_cast<uint32_t>(TraceEventFlag::Information) |
		// static_cast<uint32_t>(TraceEventFlag::HighPriority) |
		static_cast<uint32_t>(TraceEventFlag::Warning) |
		static_cast<uint32_t>(TraceEventFlag::Error) |
		static_cast<uint32_t>(TraceEventFlag::Critical);
	auto filter = std::make_shared<EventTypeFilter>(
		static_cast<TraceEventFlag>(defaultTypes));

	// Setup the console listener
	Log::RegisterListener(
		std::make_shared<ConsoleTraceListener>(
			"Log",
			filter,
			false,
			false));

	// Setup the real services
	System::ISystem::Register(std::make_shared<System::STLSystem>());
	System::IFileSystem::Register(std::make_shared<System::STLFileSystem>());

	auto workingDirectory = Path("/home/mwasplund/repos/soup/code/tools/view/");
	auto globalParameters = ValueTable();
	auto packageProvider = LoadGraph(workingDirectory, globalParameters);

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