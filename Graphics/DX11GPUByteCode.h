#pragma once

#include <wrl/client.h>

#include "DX11Headers.h"

namespace aoe{

class DX11GPUByteCode {
public:
	bool IsNull() const;
	ID3DBlob* GetNative() const;

	void* GetBufferPointer() const;
	size_t GetBufferSize() const;

	DX11GPUByteCode(ID3DBlob* byte_code);

private:
	Microsoft::WRL::ComPtr<ID3DBlob> byte_code_;
};

} // namespace aoe