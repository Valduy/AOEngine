#pragma once

#include <vector>
#include <initializer_list>

#include "DXHelper.h"

namespace aoe {

struct LayoutElement {
	LayoutElementSemantics sematic;
	GPUPixelFormat format;
};

class GPULayoutDescriptor {
public:
	const D3D11_INPUT_ELEMENT_DESC* GetBufferPointer() const {
		return layout_.data();
	}

	size_t GetBufferSize() const {
		return layout_.size();
	}

	GPULayoutDescriptor(std::initializer_list<LayoutElement> list) {
		for (const LayoutElement& element : list) {
			Add(element);
		}
	}

	void Add(const LayoutElement& element) {
		layout_.emplace_back(D3D11_INPUT_ELEMENT_DESC{
			DXHelper::ToSemanticName(element.sematic),
			0,
			DXHelper::ToDxgiFormat(element.format),
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0,
		});
	}

private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout_;
};

} // namespace aoe