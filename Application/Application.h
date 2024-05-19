#pragma once

#include "../Core/Debug.h"

#include "Executor.h"
#include "Window.h"

namespace aoe {

class Application {
public:
	Application(const std::wstring& window_name, int32_t width, int32_t height);
	~Application();

	IWindow& GetWindow();
	const IInput& GetInput() const;

	void Start(IScene& game);
	void Stop();

private:
	HINSTANCE hinstance_;
	Window window_;

	bool is_stoping_;

	void OnWindowClosing();
	void OnWindowDestroying();
};

} // namespace aoe