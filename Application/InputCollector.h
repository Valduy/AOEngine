#pragma once

#include <Windows.h>
#include <hidusage.h>

#include <vector>

#include "IInput.h"
#include "KeyState.h"

namespace aoe {

class InputCollector : public IInput {
public:
	InputCollector();

	bool IsKeyDown(Key key) const override;
	bool IsKeyHeld(Key key) const override;
	bool IsKeyUp(Key key) const override;
	Vector2i GetMouseDelta() const override;

	bool Register(HWND handle);
	void Reset();
	void ProcessWndMessage(LPARAM lparam);

private:
	std::vector<Key> released_keys_;
	KeyState key_states_[MAXBYTE];
	Vector2i mouse_delta_;

	KeyState GetKeyState(Key key) const;
	void SetKeyState(Key key, KeyState state);

	RAWINPUT ExtractRawInput(LPARAM lparam);
	void ProcessRawMouse(const RAWMOUSE mouse);
	void ProcessRawKeyboard(const RAWKEYBOARD& keyboard);

	bool IsRawKeyDown(SHORT key_flags);
	bool IsRawKeyUp(SHORT key_flags);

	void RawKeyDown(SHORT key_code);
	void RawKeyUp(USHORT key_code);

	static Key KeyCodeToKey(SHORT key_code);
	static bool IsHeld(KeyState state);
};

}