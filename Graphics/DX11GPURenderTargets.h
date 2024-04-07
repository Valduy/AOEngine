#pragma once

#include "DX11GPUTextureView.h"

namespace aoe {

struct DX11GPURenderTargets {
	static const uint32_t kRenderTargetsCount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

	const DX11GPUTextureView* render_target_views[kRenderTargetsCount] = { nullptr };
	const DX11GPUTextureView* depth_stencil_view = nullptr;
};

} // namespace aoe