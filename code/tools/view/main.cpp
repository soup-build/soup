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

struct AppState
{
	int left_size;
	int right_size;
	int top_size;
	int bottom_size;

	int tab_selected;
};

void InitializeState(AppState& state)
{
	state.left_size = 20;
	state.right_size = 20;
	state.top_size = 10;
	state.bottom_size = 10;

	state.tab_selected = 0;
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
	InitializeState(state);

	auto app = App::Fullscreen();

	std::vector<std::string> tab_values;

	for (auto& [key, value] : packageProvider.GetPackageLookup())
	{
		tab_values.push_back(value.Name.ToString());
	}

	auto tab_menu = VMenu1(&tab_values, &state.tab_selected);
	auto rendererMenu = Renderer(tab_menu, [&] {
		return tab_menu->Render() | vscroll_indicator | frame;
	});

	std::vector<std::string> tab_1_entries{
		"Forest",
		"Water",
		"I don't know",
	};
	int tab_1_selected = 0;

	std::vector<std::string> tab_2_entries{
		"Hello",
		"Hi",
		"Hay",
	};
	int tab_2_selected = 0;

	std::vector<std::string> tab_3_entries{
		"Table",
		"Nothing",
		"Is",
		"Empty",
	};
	int tab_3_selected = 0;
	auto tab_container = Container::Tab(
		{
			Radiobox(&tab_1_entries, &tab_1_selected),
			Radiobox(&tab_2_entries, &tab_2_selected),
			Radiobox(&tab_3_entries, &tab_3_selected),
		},
		&state.tab_selected);

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