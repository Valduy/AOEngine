#pragma once

#include <vector>

#include "Vertex.h"

namespace aoe {

using Index = int32_t;

class Mesh {
public:
	Mesh(std::vector<Vertex> vertices, std::vector<Index> indices)
		: vertices_(std::move(vertices))
		, indices_(std::move(indices))
	{}

	const std::vector<Vertex>& GetVertices() const {
		return vertices_;
	}

	const std::vector<Index>& GetIndices() const {
		return indices_;
	}

private:
	std::vector<Vertex> vertices_;
	std::vector<Index> indices_;
};

} // namespace aoe