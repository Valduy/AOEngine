#include "pch.h"

#include "DX11RasterizerStates.h"

namespace aoe {

DX11RasterizerStates::DX11RasterizerStates() {
	CreateRasterizerState(GPUCullMode::kNone, GPUFillMode::kWireframe);
	CreateRasterizerState(GPUCullMode::kFront, GPUFillMode::kWireframe);
	CreateRasterizerState(GPUCullMode::kBack, GPUFillMode::kWireframe);
	CreateRasterizerState(GPUCullMode::kNone, GPUFillMode::kSolid);
	CreateRasterizerState(GPUCullMode::kFront, GPUFillMode::kSolid);
	CreateRasterizerState(GPUCullMode::kBack, GPUFillMode::kSolid);
}

const DX11GPURasterizerState& DX11RasterizerStates::Get(GPUCullMode cull_mode, GPUFillMode fill_mode) const {
	DX11RasterizerStateID id{ cull_mode, fill_mode };
	return Get(id);
}

const DX11GPURasterizerState& DX11RasterizerStates::Get(DX11RasterizerStateID id) const {
	auto it = rasterizer_states_.find(id);
	AOE_ASSERT_MSG(it != rasterizer_states_.end(), "There is no rasterizer state with providet options.");

	return it->second;
}

void DX11RasterizerStates::CreateRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode) {
	DX11RasterizerStateID id{ cull_mode, fill_mode };
	GPURasterizerStateDescription desc{ cull_mode, fill_mode };
	rasterizer_states_.emplace(id, desc);
}

} // namespace aoe