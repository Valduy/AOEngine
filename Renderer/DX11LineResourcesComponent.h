#pragma once

#include <vector>

#include "../Graphics/DX11GPUBuffer.h"

namespace aoe {

class DX11LineResourcesComponent {
public:
	DX11LineResourcesComponent(std::vector<DX11GPUBuffer> line_resources)
		: line_resources_(std::move(line_resources))
	{}

	const std::vector<DX11GPUBuffer>& GetLineResources() {
		return line_resources_;
	}

private:
	std::vector<DX11GPUBuffer> line_resources_;
};

} // namespace aoe