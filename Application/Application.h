#pragma once

#include "../Core/Debug.h"
#include "../Game/Executor.h"

#include "Window.h"

namespace aoe {

class Application {
public:
	const Window& GetWindow() const {
		return window_;
	}

	Executor* GetExecutor() const {
		return executor_;
	}

	void SetExecutor(Executor* executor) {
		executor_ = executor;
	}

	Application(const std::wstring& window_name, int32_t width, int32_t height)
		: hinstance_(GetModuleHandle(nullptr))
		, window_(hinstance_, window_name, width, height)
		, executor_(nullptr)
	{}

	void Start() {
		AOE_ASSERT_MSG(executor_ != nullptr, "Executor is not setuped.");

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
		// TODO: implement
		AOE_ASSERT_MSG(false, "Not implemented.");
	}

private:
	HINSTANCE hinstance_;
	Window window_;
	Executor* executor_;
};

} // namespace aoe