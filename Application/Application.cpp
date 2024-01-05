#include "pch.h"

#include "Application.h"

namespace aoe {

const Window& Application::GetWindow() const {
	return window_;
}

Application::Application(const std::wstring& window_name, int32_t width, int32_t height)
	: hinstance_(GetModuleHandle(nullptr))
	, window_(hinstance_, window_name, width, height)
	, is_stoping_(false)
{}

void Application::Start(IGame& game) {
	MSG msg = {};
	Executor executor(game);

	is_stoping_ = false;
	window_.Show();
	executor.Initialize();
	executor.ResetDeltaTime();

	while (!is_stoping_) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				is_stoping_ = true;
			}
		}

		executor.Tick();
	}

	executor.Terminate();
}

void Application::Stop() {
	is_stoping_ = true;
}

} // namespace aoe
