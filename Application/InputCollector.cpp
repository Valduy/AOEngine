#include "pch.h"

#include "InputCollector.h"

namespace aoe {

InputCollector::InputCollector()
	: released_keys_()
	, key_states_{ KeyState::kIdle }
	, mouse_delta_(Math::kZeros2i)
{}

bool InputCollector::IsKeyDown(Key key) const {
	KeyState state = GetKeyState(key);
	return state == KeyState::kDown;
}

bool InputCollector::IsKeyHeld(Key key) const {
	KeyState state = GetKeyState(key);
	return state == KeyState::kDown || state == KeyState::kHeld;
}

bool InputCollector::IsKeyUp(Key key) const {
	KeyState state = GetKeyState(key);
	return state == KeyState::kUp;
}

Vector2i InputCollector::GetMouseDelta() const {
	return mouse_delta_;
}

bool InputCollector::Register(HWND handle) {
	const size_t devices_count = 2;
	RAWINPUTDEVICE raw_input_device[devices_count];

	raw_input_device[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	raw_input_device[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	raw_input_device[0].dwFlags = 0;
	raw_input_device[0].hwndTarget = handle;

	raw_input_device[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	raw_input_device[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	raw_input_device[1].dwFlags = 0;
	raw_input_device[1].hwndTarget = handle;

	return RegisterRawInputDevices(raw_input_device, devices_count, sizeof(RAWINPUTDEVICE));
}

void InputCollector::Reset() {
	for (Key key : released_keys_) {
		SetKeyState(key, KeyState::kIdle);
	}

	released_keys_.clear();
	mouse_delta_ = Math::kZeros2i;
}

void InputCollector::ProcessWndMessage(LPARAM lparam) {
	const RAWINPUT raw = ExtractRawInput(lparam);

	switch (raw.header.dwType) {
	case RIM_TYPEMOUSE: {
		ProcessRawMouse(raw.data.mouse);
		return;
	}
	case RIM_TYPEKEYBOARD: {
		ProcessRawKeyboard(raw.data.keyboard);
		return;
	}
	}
}

KeyState InputCollector::GetKeyState(Key key) const {
	size_t idx = static_cast<size_t>(key);
	return key_states_[idx];
}

void InputCollector::SetKeyState(Key key, KeyState state) {
	size_t idx = static_cast<size_t>(key);
	key_states_[idx] = state;
}

RAWINPUT InputCollector::ExtractRawInput(LPARAM lparam) {
	UINT size = sizeof(RAWINPUT);
	RAWINPUT raw;

	const HRAWINPUT raw_handle = reinterpret_cast<HRAWINPUT>(lparam);
	GetRawInputData(raw_handle, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

	return raw;
}

void InputCollector::ProcessRawMouse(const RAWMOUSE mouse) {
	int mouse_dx = static_cast<int>(mouse.lLastX);
	int mouse_dy = static_cast<int>(mouse.lLastY);
	mouse_delta_ = { mouse_dx, mouse_dy };
}

void InputCollector::ProcessRawKeyboard(const RAWKEYBOARD& keyboard) {
	if (IsRawKeyDown(keyboard.Flags)) {
		RawKeyDown(keyboard.VKey);
	}
	if (IsRawKeyUp(keyboard.Flags)) {
		RawKeyUp(keyboard.VKey);
	}
}

bool InputCollector::IsRawKeyDown(SHORT key_flags) {
	return key_flags == RI_KEY_MAKE;
}

bool InputCollector::IsRawKeyUp(SHORT key_flags) {
	return key_flags & RI_KEY_BREAK;
}

void InputCollector::RawKeyDown(SHORT key_code) {
	Key key = KeyCodeToKey(key_code);
	KeyState prev_state = GetKeyState(key);
	KeyState next_state = IsHeld(prev_state) ? KeyState::kHeld : KeyState::kDown;
	SetKeyState(key, next_state);
}

void InputCollector::RawKeyUp(USHORT key_code) {
	Key key = KeyCodeToKey(key_code);
	SetKeyState(key, KeyState::kUp);
	released_keys_.push_back(key);
}

Key InputCollector::KeyCodeToKey(SHORT key_code) {
	return static_cast<Key>(key_code);
}

bool InputCollector::IsHeld(KeyState state) {
	return state == KeyState::kDown || state == KeyState::kHeld;
}

} // namespace aoe