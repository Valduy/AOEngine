#pragma once

#include <wrl/client.h>

#include "DXHelper.h"

namespace aoe{

class DXByteCode {
public:
	bool IsNull() const {
		return byte_code_ == nullptr;
	}

	void* GetBufferPointer() const {
		return byte_code_ != nullptr ? byte_code_->GetBufferPointer() : nullptr;
	}

	size_t GetBufferSize() const {
		return byte_code_ != nullptr ? byte_code_->GetBufferSize() : 0;
	}

	DXByteCode(ID3DBlob* byte_code)
		: byte_code_(byte_code)
	{}

private:
	Microsoft::WRL::ComPtr<ID3DBlob> byte_code_;
};

} // namespace aoe