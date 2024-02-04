#pragma once

#include "IGPURasterizerState.h"
#include "DX11GPUDevice.h"

namespace aoe {

class DX11GPURasterizerState : public IGPURasterizerState {
public:
	void* GetNative() const override;
	const GPURasterizerStateDescription& GetDescription() const override;

	DX11GPURasterizerState(const DX11GPUDevice& device, const GPURasterizerStateDescription& description);

private:
	const DX11GPUDevice& device_;
	GPURasterizerStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state_;

	static D3D11_CULL_MODE ToDXCullMode(const GPUCullMode value);
	static D3D11_FILL_MODE ToDXFillMode(const GPUFillMode value);
};

} // namespace aoe