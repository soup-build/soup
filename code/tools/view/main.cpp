#include <memory>
#include <optional>
#include <string>

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
};

void InitializeState(AppState& state)
{
	state.left_size = 20;
	state.right_size = 20;
	state.top_size = 10;
	state.bottom_size = 10;
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

	auto state = AppState();
	InitializeState(state);

	auto app = App::Fullscreen();
	
	std::string label = "Click to quit";
	auto slider = Slider("Value:", &state.top_size, 0, 100, 1);
	auto button = Button(&label, app.ExitLoopClosure());
	auto renderer2 = Renderer(button, [&] {
		return hbox({
		text("A button:"),
		button->Render(),
		slider->Render(),
		});
	});

	// Renderers:
	auto RendererInfo = [](const std::string& name, int* size) {
		return Renderer([name, size] {
		return text(name + ": " + std::to_string(*size)) | center;
		});
	};
	auto middle = renderer2;//Renderer([] { return text("Middle") | center; });
	auto left = RendererInfo("Left", &state.left_size);
	auto right = RendererInfo("Right", &state.right_size);
	auto top = RendererInfo("Top", &state.top_size);
	auto bottom = RendererInfo("Bottom", &state.bottom_size);

	auto container = middle;
	container = ResizableSplitLeft(left, container, &state.left_size);
	container = ResizableSplitRight(right, container, &state.right_size);
	container = ResizableSplitTop(top, container, &state.top_size);
	container = ResizableSplitBottom(bottom, container, &state.bottom_size);

	auto renderer = Renderer(container, [&] { return container->Render() | border; });

	app.Loop(renderer);
}