#pragma once

#include "../Core/Debug.h"

#include "GPUDevice.h"
#include "GPUPixelFormat.h"

namespace aoe {

class GPUDevice;

struct GPUDepthStateDescription {
	bool is_depth_enabled;
	GPUDepthWriteMask write_mask;
	GPUComparsionFunction comparsion_function;

	GPUDepthStateDescription()
		: is_depth_enabled(false)
		, write_mask(GPUDepthWriteMask::kWriteNone)
		, comparsion_function(GPUComparsionFunction::kNever)
	{}
};

class GPUDepthState : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUDepthState)

	ID3D11DepthStencilState* GetNative() const;
	
	const GPUDepthStateDescription& GetDescription() const;

	GPUDepthState(const GPUDevice& device);

	bool Initialize(const GPUDepthStateDescription& description);
	void Terminate() override;

private:
	const GPUDevice& device_;
	GPUDepthStateDescription description_;
	ID3D11DepthStencilState* depth_stencil_state_;

	static D3D11_DEPTH_WRITE_MASK ToDXDepthWirteMask(const GPUDepthWriteMask value);
	static D3D11_COMPARISON_FUNC ToDXComparsionFunc(const GPUComparsionFunction value);
};

} // namespace aoe