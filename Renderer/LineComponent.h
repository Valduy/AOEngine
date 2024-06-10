#pragma once

#include <vector>

#include "Colors.h"
#include "Line.h"

namespace aoe {

class LineComponent {
public:
	Vector3f color;

	LineComponent(Vector3f color, Line line)
		: color(color)
		, line_(std::move(line))
	{}

	const Line& GetLine() const {
		return line_;
	}

private:
	Line line_;
};

} // namespace aoe