#pragma once

#include <wrl/client.h>

#include "../Core/Singleton.h"

#include "DX11Headers.h"

namespace aoe {

class DX11GPUContext;

class DX11GPUDevice : public Singleton<DX11GPUDevice> {
public:
	DX11GPUDevice();

	ID3D11Device* GetNative() const;
	DX11GPUContext GetContext() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
};

} // namespace aoe