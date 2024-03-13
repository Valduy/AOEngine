#include "pch.h"

#include "DX11GPUByteCode.h"

namespace aoe {

DX11GPUByteCode::DX11GPUByteCode(ID3DBlob* byte_code)
	: byte_code_(byte_code)
{}

bool DX11GPUByteCode::IsNull() const {
	return byte_code_ == nullptr;
}

ID3DBlob* DX11GPUByteCode::GetNative() const {
	return byte_code_.Get();
}

void* DX11GPUByteCode::GetBufferPointer() const {
	return byte_code_ != nullptr ? byte_code_->GetBufferPointer() : nullptr;
}

size_t DX11GPUByteCode::GetBufferSize() const {
	return byte_code_ != nullptr ? byte_code_->GetBufferSize() : 0;
}

} // namespace aoe
