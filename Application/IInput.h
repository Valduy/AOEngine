#pragma once

#include "../Core/Math.h"

#include "Key.h"

namespace aoe {

class IInput {
public:
	virtual ~IInput() = default;
	virtual bool IsKeyDown(Key key) const = 0;
	virtual bool IsKeyHeld(Key key) const = 0;
	virtual bool IsKeyUp(Key key) const = 0;
	virtual Vector2i GetMouseDelta() const = 0;
};

} // namespace aoe