#pragma once

#include "IGPUDepthState.h"
#include "DX11GPUDevice.h"

namespace aoe {

class DX11GPUDepthState : public IGPUDepthState {
public:
	void* GetNative() const override;
	const GPUDepthStateDescription& GetDescription() const override;

	DX11GPUDepthState(const DX11GPUDevice& device, GPUDepthStateDescription description);

private:
	const DX11GPUDevice& device_;
	GPUDepthStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state_;

	static D3D11_DEPTH_WRITE_MASK ToDXDepthWirteMask(const GPUDepthWriteMask value);
	static D3D11_COMPARISON_FUNC ToDXComparsionFunc(const GPUComparsionFunction value);
};

} // namespace aoe