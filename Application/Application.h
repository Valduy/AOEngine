#pragma once

#include <cassert>

#include "../Game/Executor.h"

#include "Window.h"

namespace aoe {

class Application {
public:
	Executor* GetExecutor() const {
		return executor_;
	}

	void SetExecutor(Executor* executor) {
		executor_ = executor;
	}

	Application(LPCWSTR window_name, LONG width, LONG height)
		: hinstance_(GetModuleHandle(nullptr))
		, window_(hinstance_, window_name, width, height)
		, executor_(nullptr)
	{}

	void Start() {
		assert(executor_ != nullptr, "Executor is not setuped.");

		MSG msg = {};
		bool is_exit_requested = false;

		window_.Show();
		executor_->Initialize();
		executor_->ResetDeltaTime();

		while (!is_exit_requested) {
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					is_exit_requested = true;
				}
			}

			executor_->Tick();
		}

		executor_->Terminate();
	}

	void Stop() {
		// TODO: implementation
		throw "Not implemented.";
	}

private:
	HINSTANCE hinstance_;
	Window window_;
	Executor* executor_;
};

} // namespace aoe