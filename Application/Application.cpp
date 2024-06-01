#include "pch.h"

#include "Application.h"

namespace aoe {

Application::Application(const std::wstring& window_name, int32_t width, int32_t height)
	: hinstance_(GetModuleHandle(nullptr))
	, window_(hinstance_, window_name, width, height)
	, is_stoping_(false)
{
	window_.Closing.Attach(*this, &Application::OnWindowClosing);
	window_.Destroying.Attach(*this, &Application::OnWindowDestroying);
	window_.ShowWindow(true);
}

Application::~Application() {
	window_.Closing.Detach(*this, &Application::OnWindowClosing);
	window_.Destroying.Detach(*this, &Application::OnWindowDestroying);
}

IWindow& Application::GetWindow() {
	return window_;
}

const IInput& Application::GetInput() const {
	return window_.GetInput();
}

void Application::Start(IScene& scene) {
	MSG msg = {};
	Executor executor(scene);

	is_stoping_ = false;

	while (!is_stoping_) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_DESTROY) {
				is_stoping_ = true;
			}
		}

		executor.Tick();
		window_.Tick();
	}
}

void Application::Stop() {
	is_stoping_ = true;
}

void Application::OnWindowClosing() {
	Stop();
}

void Application::OnWindowDestroying() {
	Stop();
}

} // namespace aoe
