#pragma once

#include "GPUDevice.h"
#include "GPUEnums.h"

namespace aoe {

struct GPURasteriserStateDescription {
	GPUCullMode cull_mode;
	GPUFillMode fill_mode;

	GPURasteriserStateDescription()
		: cull_mode(GPUCullMode::kBack)
		, fill_mode(GPUFillMode::kSolid)
	{}
};

class GPURasterizerState : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPURasterizerState)

	ID3D11RasterizerState* GetNative() const;
	const GPURasteriserStateDescription& GetDescription() const;

	GPURasterizerState(const GPUDevice& device);

	bool Initialize(const GPURasteriserStateDescription& description);
	void Terminate() override;

private:
	const GPUDevice& device_;
	GPURasteriserStateDescription description_;
	ID3D11RasterizerState* rasterizer_state_;

	static D3D11_CULL_MODE ToDXCullMode(const GPUCullMode value);
	static D3D11_FILL_MODE ToDXFillMode(const GPUFillMode value);
};

} // namespace aoe