#pragma once

#include "../Core/Event.h"

namespace aoe {

enum class MouseMode {
	kAbsolute,
	kRelative,
};

class IWindow {
public:
	Event<IWindow> Closing;
	Event<IWindow> Destroying;

	virtual ~IWindow() = default;
	virtual void* GetNative() const = 0;

	virtual int32_t GetWidth() const = 0;
	virtual int32_t GetHeight() const = 0;

	virtual MouseMode GetMouseMode() const = 0;
	virtual void SetMouseMode(MouseMode value) = 0;

	virtual void ShowWindow(bool is_shown) = 0;

protected:
	void FireClosing() {
		Closing.Notify();
	}

	void FireDestroying() {
		Destroying.Notify();
	}
};

} // namespace aoe