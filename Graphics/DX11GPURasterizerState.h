#pragma once

#include "GPUEnums.h"
#include "DX11GPUDevice.h"

namespace aoe {

struct GPURasterizerStateDescription {
	GPUCullMode cull_mode;
	GPUFillMode fill_mode;

	GPURasterizerStateDescription(GPUCullMode cull_mode, GPUFillMode fill_mode)
		: cull_mode(cull_mode)
		, fill_mode(fill_mode)
	{}

	GPURasterizerStateDescription()
		: cull_mode(GPUCullMode::kBack)
		, fill_mode(GPUFillMode::kSolid)
	{}
};

class DX11GPURasterizerState {
public:
	DX11GPURasterizerState(const GPURasterizerStateDescription& description);

	ID3D11RasterizerState* GetNative() const;
	const GPURasterizerStateDescription& GetDescription() const;

private:
	GPURasterizerStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state_;

	static D3D11_CULL_MODE ToDXCullMode(const GPUCullMode value);
	static D3D11_FILL_MODE ToDXFillMode(const GPUFillMode value);
};

} // namespace aoe