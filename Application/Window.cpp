#include "pch.h"

#include "Window.h"

namespace aoe {

HWND Window::GetHandler() const {
	return handler_;
}

int32_t Window::GetWidth() const {
	RECT rect;
	GetWindowRect(handler_, &rect);
	return static_cast<int32_t>(rect.right - rect.left);
}

int32_t Window::GetHeight() const {
	RECT rect;
	GetWindowRect(handler_, &rect);
	return static_cast<int32_t>(rect.bottom - rect.top);
}

Window::Window(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height)
	: handler_(0)
{
	if (!RegisterWindowClass(hinstance, window_name)) {
		Platform::Fatal("Failed register window class.");
	}

	handler_ = CreateWindowInstance(hinstance, window_name, width, height);

	if (handler_ == 0) {
		Platform::Fatal("Failed to create window.");
	}
}

Window::~Window() {
	if (handler_ != 0) {
		if (DestroyWindow(handler_) == 0) {
			AOE_LOG_ERROR("DestroyWindow failed! Error: {}", GetLastError());
		}

		handler_ = 0;
	}
}

void Window::Show() {
	ShowWindow(handler_, SW_SHOW);
	//SetForegroundWindow(handler_);
	//SetFocus(handler_);
	//ShowCursor(false);
}

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	Window* window = nullptr;

	if (msg == WM_CREATE) {
		const auto create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
		window = static_cast<Window*>(create_struct->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	}
	else {
		window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	switch (msg) {
	case WM_DESTROY:
		// TODO: handle
	case WM_CLOSE:
		// TODO: handle
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

bool Window::RegisterWindowClass(HINSTANCE hinstance, const std::wstring& window_name) {
	WNDCLASSEX window_class;
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = WndProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = hinstance;
	window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	window_class.hIconSm = window_class.hIcon;
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	window_class.lpszMenuName = nullptr;
	window_class.lpszClassName = window_name.data();
	window_class.cbSize = sizeof(WNDCLASSEX);

	return RegisterClassEx(&window_class);
}

HWND Window::CreateWindowInstance(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height) {
	RECT window_rect = { 0, 0, width, height };
	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	int window_width = static_cast<int>(window_rect.right - window_rect.left);
	int window_height = static_cast<int>(window_rect.bottom - window_rect.top);
	int window_position_x = (screen_width - window_width) / 2;
	int window_position_y = (screen_height - window_height) / 2;

	return CreateWindowEx(
		WS_EX_APPWINDOW,
		window_name.data(),
		window_name.data(),
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME,
		window_position_x,
		window_position_y,
		window_width,
		window_height,
		nullptr,
		nullptr,
		hinstance,
		this);
}

} // namespace aoe
