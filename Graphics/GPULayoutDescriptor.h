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
	const D3D11_INPUT_ELEMENT_DESC* GetBufferPointer() const;
	size_t GetBufferSize() const;

	GPULayoutDescriptor(std::initializer_list<LayoutElement> list);

	void Add(const LayoutElement& element);

private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout_;

	static const char* ToSemanticName(LayoutElementSemantics value);
};

} // namespace aoe