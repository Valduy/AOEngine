#pragma once

#include <Windows.h>

#include <string>

#include "../Core/Logger.h"
#include "../Core/Event.h"

#include "IWindow.h"
#include "InputCollector.h"

namespace aoe {

class Window : public IWindow {
public:
	Window(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height);
	~Window();

	HWND GetHandle() const;
	const IInput& GetInput() const;
	void Tick();

	void* GetNative() const override;

	int32_t GetWidth() const override;
	int32_t GetHeight() const override;

	MouseMode GetMouseMode() const override;
	void SetMouseMode(MouseMode value) override;

	void ShowWindow(bool is_shown) override;

private:
	HWND handle_;
	MouseMode mouse_mode_;
	InputCollector input_collector_;

	int32_t width_;
	int32_t height_;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	bool RegisterWindowClass(HINSTANCE hinstance, const std::wstring& window_name);
	HWND CreateWindowInstance(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height);

	void UpdateMouseMode();
	void SetupAbsoluteMouseMode();
	void SetupRelativeMouseMode();
	void ClipToWindow();
	void UnclipFromWindow();
};

} // namespace aoe