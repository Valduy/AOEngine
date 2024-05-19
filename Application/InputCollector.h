#pragma once

#include "Window.h"

#include <hidusage.h>

#include "IInput.h"
#include "KeyState.h"
#include "Platform.h"

namespace aoe {

class InputCollector : public IInput {
public:
	InputCollector()
		: key_states_{ KeyState::kIdle }
		, released_keys_()
		, mouse_delta_(Math::kZeros2i)
	{}

	bool IsKeyDown(Key key) const override {
		KeyState state = GetKeyState(key);
		return state == KeyState::kDown;
	}

	bool IsKeyHeld(Key key) const override {
		KeyState state = GetKeyState(key);
		return state == KeyState::kHeld;
	}

	bool IsKeyUp(Key key) const override {
		KeyState state = GetKeyState(key);
		return state == KeyState::kUp;
	}

	Vector2i GetMouseDelta() const override {
		return mouse_delta_;
	}

	bool Register(HWND handle) {
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

	void ResetReleasedKeys() {
		for (Key key : released_keys_) {
			SetKeyState(key, KeyState::kIdle);
		}

		released_keys_.clear();
	}

	void ProcessWndMessage(LPARAM lparam) {
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

private:
	KeyState key_states_[MAXBYTE];
	std::vector<Key> released_keys_;

	Vector2i mouse_delta_;

	KeyState GetKeyState(Key key) const {
		size_t idx = static_cast<size_t>(key);
		return key_states_[idx];
	}

	void SetKeyState(Key key, KeyState state) {
		size_t idx = static_cast<size_t>(key);
		key_states_[idx] = state;
	}

	RAWINPUT ExtractRawInput(LPARAM lparam) {
		UINT size = sizeof(RAWINPUT);
		RAWINPUT raw;

		const HRAWINPUT raw_handle = reinterpret_cast<HRAWINPUT>(lparam);
		GetRawInputData(raw_handle, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

		return raw;
	}

	void ProcessRawMouse(const RAWMOUSE mouse) {
		int mouse_dx = static_cast<int>(mouse.lLastX);
		int mouse_dy = static_cast<int>(mouse.lLastY);
		mouse_delta_ = { mouse_dx, mouse_dy };
	}

	void ProcessRawKeyboard(const RAWKEYBOARD& keyboard) {
		if (IsRawKeyDown(keyboard.Flags)) {
			RawKeyDown(keyboard.VKey);
		}
		if (IsRawKeyUp(keyboard.Flags)) {
			RawKeyUp(keyboard.VKey);
		}
	}

	bool IsRawKeyDown(SHORT key_flags) {
		return key_flags == RI_KEY_MAKE;
	}

	bool IsRawKeyUp(SHORT key_flags) {
		return key_flags & RI_KEY_BREAK;
	}

	void RawKeyDown(SHORT key_code) {
		Key key = KeyCodeToKey(key_code);
		KeyState prev_state = GetKeyState(key);
		KeyState next_state = IsHolding(prev_state) ? KeyState::kHeld : KeyState::kDown;
		SetKeyState(key, next_state);
	}

	void RawKeyUp(USHORT key_code) {
		Key key = KeyCodeToKey(key_code);
		SetKeyState(key, KeyState::kUp);
		released_keys_.push_back(key);
	}

	static Key KeyCodeToKey(SHORT key_code) {
		return static_cast<Key>(key_code);
	}

	static bool IsHolding(KeyState state) {
		return state == KeyState::kDown || state == KeyState::kHeld;
	}
};

}