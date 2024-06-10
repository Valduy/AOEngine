#pragma once

#include <vector>

#include "../Core/Math.h"

namespace aoe {

class Segment {
public:
	Segment(std::vector<Vector3f> points)
		: points_(std::move(points))
	{}

	const std::vector<Vector3f>& GetPoints() const {
		return points_;
	}

private:
	std::vector<Vector3f> points_;
};

class Line {
public:
	Line(std::vector<Segment> segments)
		: segments_(segments)
	{}

	const std::vector<Segment>& GetSegments() const {
		return segments_;
	}

private:
	std::vector<Segment> segments_;
};

}