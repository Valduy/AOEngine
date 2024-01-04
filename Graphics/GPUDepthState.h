#pragma once

#include "../Core/Debug.h"

#include "GPUDevice.h"
#include "GPUPixelFormat.h"

namespace aoe {

class GPUDevice;

class GPUDepthState : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUDepthState)

	ID3D11DepthStencilState* GetNative() const;

	GPUDepthState(const GPUDevice& device);

	bool Initialize(bool is_depth_enabled, GPUDepthWriteMask write_mask, GPUComparsionFunc comparsion_func);
	void Terminate() override;

private:
	ID3D11DepthStencilState* depth_stencil_state_;

	const GPUDevice& device_;
};

} // namespace aoe