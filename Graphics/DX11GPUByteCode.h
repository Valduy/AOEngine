#pragma once

#include <wrl/client.h>

#include "DX11Headers.h"

namespace aoe{

class DX11GPUByteCode {
public:
	DX11GPUByteCode(ID3DBlob* byte_code);

	bool IsNull() const;
	ID3DBlob* GetNative() const;

	void* GetBufferPointer() const;
	size_t GetBufferSize() const;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> byte_code_;
};

} // namespace aoe