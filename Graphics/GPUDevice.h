#pragma once

#include <wrl/client.h>

#include "../Core/ClassHelper.h"

#include "DXHelper.h"
#include "IGPUResource.h"

namespace aoe {

class GPUContext;

class GPUDevice {
public:
	ID3D11Device* GetNative() const;
	GPUContext GetContext() const;

	GPUDevice();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
};

} // namespace aoe