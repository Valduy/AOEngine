#pragma once

#include "../Core/Debug.h"

#include "Executor.h"
#include "Window.h"

namespace aoe {

class Application {
public:
	Application(const std::wstring& window_name, int32_t width, int32_t height);

	const Window& GetWindow() const;

	void Start(IScene& game);
	void Stop();

private:
	HINSTANCE hinstance_;
	Window window_;

	bool is_stoping_;
};

} // namespace aoe