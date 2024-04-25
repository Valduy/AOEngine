#pragma once

#include "GPUEnums.h"
#include "DX11GPURenderTargets.h"

namespace aoe {

struct GPURenderTargetBlendStateDesc {
	bool is_blend_enable;
	GPUBlend source_blend;
	GPUBlend destination_blend;
	GPUBlendOperation blend_operation;
	GPUBlend source_blend_alpha;
	GPUBlend destination_blend_alpha;
	GPUBlendOperation blend_operation_alpha;
	GPUColorWriteMask color_write_mask;

	GPURenderTargetBlendStateDesc()
		: is_blend_enable(false)
		, source_blend(GPUBlend::kZero)
		, destination_blend(GPUBlend::kZero)
		, blend_operation(GPUBlendOperation::kAdd)
		, source_blend_alpha(GPUBlend::kZero)
		, destination_blend_alpha(GPUBlend::kZero)
		, blend_operation_alpha(GPUBlendOperation::kAdd)
		, color_write_mask(GPUColorWriteMask::kAll)
	{}
};

struct GPUBlendStateDescription {
	bool is_alpha_to_coverage_enable;
	bool is_independent_blend_enable;
	GPURenderTargetBlendStateDesc render_targets[DX11GPURenderTargets::kRenderTargetsCount];

	GPUBlendStateDescription()
		: is_alpha_to_coverage_enable(false)
		, is_independent_blend_enable(false)
		, render_targets()
	{}
};

class DX11GPUBlendState {
public:
	DX11GPUBlendState(const GPUBlendStateDescription description);

	ID3D11BlendState* GetNative() const;
	const GPUBlendStateDescription& GetDescription() const;

private:
	GPUBlendStateDescription description_;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state_;

	static D3D11_BLEND ToDXBlend(const GPUBlend value);
	static D3D11_BLEND_OP ToDXBlendOperation(const GPUBlendOperation value);
	static uint8_t ToDXColorWriteMask(const GPUColorWriteMask value);
};

} // namespace aoe