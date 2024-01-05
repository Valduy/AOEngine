#pragma once

#include <Windows.h>

#include <string>

#include "../Core/Logger.h"

#include "Platform.h"

namespace aoe {

class Window {
public:
	HWND GetHandler() const;
	
	int32_t GetWidth() const;
	int32_t GetHeight() const;

	Window(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height);
	~Window();

	void Show();

private:
	HWND handler_;

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	bool RegisterWindowClass(HINSTANCE hinstance, const std::wstring& window_name);
	HWND CreateWindowInstance(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height);
};

} // namespace aoe