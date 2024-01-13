#pragma once

#include "GPUDevice.h"
#include "GPUEnums.h"

namespace aoe {

struct GPURasteriserStateDescription {
	GPUCullMode cull_mode;
	GPUFillMode fill_mode;

	GPURasteriserStateDescription(GPUCullMode cull_mode, GPUFillMode fill_mode)
		: cull_mode(cull_mode)
		, fill_mode(fill_mode)
	{}

	GPURasteriserStateDescription()
		: cull_mode(GPUCullMode::kBack)
		, fill_mode(GPUFillMode::kSolid)
	{}
};

class GPURasterizerState {
public:
	ID3D11RasterizerState* GetNative() const;
	const GPURasteriserStateDescription& GetDescription() const;

	GPURasterizerState(const GPUDevice& device, const GPURasteriserStateDescription& description);

private:
	const GPUDevice& device_;
	GPURasteriserStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state_;

	static D3D11_CULL_MODE ToDXCullMode(const GPUCullMode value);
	static D3D11_FILL_MODE ToDXFillMode(const GPUFillMode value);
};

} // namespace aoe