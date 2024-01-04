#pragma once

#include "GPUDevice.h"
#include "GPUEnums.h"

namespace aoe {

class GPURasterizerState : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPURasterizerState)

	ID3D11RasterizerState* GetNative() const;

	GPURasterizerState(const GPUDevice& device);

	bool Initialize(GPUCullMode cull_mode, GPUFillMode fill_mode);
	void Terminate() override;

private:
	ID3D11RasterizerState* rasterizer_state_;

	const GPUDevice& device_;
};

} // namespace aoe