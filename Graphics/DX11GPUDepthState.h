#pragma once

#include "GPUEnums.h"
#include "DX11GPUDevice.h"

namespace aoe {

struct GPUDepthStateDescription {
	bool is_depth_enabled;
	GPUDepthWriteMask write_mask;
	GPUComparsionFunction comparsion_function;

	GPUDepthStateDescription(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function
	)
		: is_depth_enabled(is_depth_enabled)
		, write_mask(write_mask)
		, comparsion_function(comparsion_function)
	{}

	GPUDepthStateDescription()
		: is_depth_enabled(false)
		, write_mask(GPUDepthWriteMask::kWriteNone)
		, comparsion_function(GPUComparsionFunction::kNever)
	{}
};

class DX11GPUDepthState {
public:
	ID3D11DepthStencilState* GetNative() const;
	const GPUDepthStateDescription& GetDescription() const;

	DX11GPUDepthState(GPUDepthStateDescription description);

private:
	GPUDepthStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state_;

	static D3D11_DEPTH_WRITE_MASK ToDXDepthWirteMask(const GPUDepthWriteMask value);
	static D3D11_COMPARISON_FUNC ToDXComparsionFunc(const GPUComparsionFunction value);
};

} // namespace aoe