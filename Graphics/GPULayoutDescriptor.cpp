#include "pch.h"

#include "GPULayoutDescriptor.h"

namespace aoe {

const D3D11_INPUT_ELEMENT_DESC* GPULayoutDescriptor::GetBufferPointer() const {
	return layout_.data();
}

size_t GPULayoutDescriptor::GetBufferSize() const {
	return layout_.size();
}

GPULayoutDescriptor::GPULayoutDescriptor(std::initializer_list<LayoutElement> list) {
	layout_.reserve(list.size());
	for (const LayoutElement& element : list) {
		Add(element);
	}
}

void GPULayoutDescriptor::Add(const LayoutElement& element) {
	layout_.emplace_back(D3D11_INPUT_ELEMENT_DESC{
		ToSemanticName(element.sematic),
		0,
		DX11Helper::ToDxgiFormat(element.format),
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	});
}

const char* GPULayoutDescriptor::ToSemanticName(LayoutElementSemantics value) {
	switch (value) {
	case aoe::LayoutElementSemantics::kBinormal:
		return "BINORMAL";
	case aoe::LayoutElementSemantics::kBlendIndices:
		return "BLENDINDICES";
	case aoe::LayoutElementSemantics::kBlendWeight:
		return "BLENDWEIGHT";
	case aoe::LayoutElementSemantics::kColor:
		return "COLOR";
	case aoe::LayoutElementSemantics::kNormal:
		return "NORMAL";
	case aoe::LayoutElementSemantics::kPosition:
		return "POSITION";
	case aoe::LayoutElementSemantics::kPositionT:
		return "POSITIONT";
	case aoe::LayoutElementSemantics::kPSize:
		return "PSIZE";
	case aoe::LayoutElementSemantics::kTangent:
		return "TANGENT";
	case aoe::LayoutElementSemantics::kTexcoord:
		return "TEXCOORD";
	default:
		AOE_ASSERT_MSG(false, "Unknown sematics {}.", static_cast<int>(value));
		return "";
	}
}

} // namespace aoe