#include "pch.h"

#include "Window.h"
#include "Platform.h"

namespace aoe {

Window::Window(HINSTANCE hinstance, const std::wstring& window_name, int32_t width, int32_t height)
	: handle_(0)
	, mouse_mode_(MouseMode::kRelative)
	, input_collector_()
	, width_(width)
	, height_(height)
{
	if (!RegisterWindowClass(hinstance, window_name)) {
		Platform::Fatal("Failed register window class.");
	}

	handle_ = CreateWindowInstance(hinstance, window_name, width, height);

	if (handle_ == 0) {
		Platform::Fatal("Failed to create window.");
	}

	if (!input_collector_.Register(handle_)) {
		Platform::Fatal("Failed to register InputCollector.");
	}
}

Window::~Window() {
	if (handle_ != 0) {
		if (DestroyWindow(handle_) == 0) {
			AOE_LOG_ERROR("DestroyWindow failed! Error: {}", GetLastError());
		}

		handle_ = 0;
	}
}

HWND Window::GetHandle() const {
	return handle_;
}

const IInput& Window::GetInput() const {
	return input_collector_;
}

void Window::Tick() {
	input_collector_.Reset();
}

void* Window::GetNative() const {
	return handle_;
}

int32_t Window::GetWidth() const {
	return width_;
}

int32_t Window::GetHeight() const {
	return height_;
}

MouseMode Window::GetMouseMode() const {
	return mouse_mode_;
}

void Window::SetMouseMode(MouseMode value) {
	mouse_mode_ = value;
	UpdateMouseMode();
}

void Window::ShowWindow(bool is_shown) {
	::ShowWindow(handle_, is_shown ? SW_SHOW : SW_HIDE);
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
	case WM_DESTROY: {
		window->FireDestroying();
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE: {
		int32_t width = LOWORD(lparam);
		int32_t height = HIWORD(lparam);

		window->width_ = width;
		window->height_ = height;
		window->FireResize(width, height);
		return 0;
	}
	case WM_CLOSE: {
		window->FireClosing();
		return 0;
	}
	case WM_EXITSIZEMOVE: {
		if (window->mouse_mode_ == MouseMode::kRelative) {
			window->ClipToWindow();
		}

		window->FireSizeChanged(window->GetWidth(), window->GetHeight());
		return 0;
	}
	case WM_ACTIVATEAPP: {
		if (wparam) {
			window->UpdateMouseMode();
		} else {
			window->SetupAbsoluteMouseMode();
		}
		
		return 0;
	}
	case WM_INPUT: {
		window->input_collector_.ProcessWndMessage(lparam);
		return 0;
	}
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

void Window::UpdateMouseMode() {
	switch (mouse_mode_) {
	case MouseMode::kAbsolute:
		SetupAbsoluteMouseMode();
		break;
	case MouseMode::kRelative:
		SetupRelativeMouseMode();
		break;
	default:
		AOE_ASSERT_MSG(false, "Unknown MouseMode.");
	}
}

void Window::SetupAbsoluteMouseMode() {
	ShowCursor(true);
	UnclipFromWindow();
}

void Window::SetupRelativeMouseMode() {
	ShowCursor(false);
	ClipToWindow();
}

void Window::ClipToWindow() {
	RECT rect;
	GetClientRect(handle_, &rect);

	POINT lt;
	lt.x = rect.left;
	lt.y = rect.top;

	POINT rb;
	rb.x = rect.right;
	rb.y = rect.bottom;

	MapWindowPoints(handle_, nullptr, &lt, 1);
	MapWindowPoints(handle_, nullptr, &rb, 1);

	rect.left = lt.x;
	rect.top = lt.y;

	rect.right = rb.x;
	rect.bottom = rb.y;

	ClipCursor(&rect);
}

void Window::UnclipFromWindow() {
	ClipCursor(nullptr);
}

} // namespace aoe
