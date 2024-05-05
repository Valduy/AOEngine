#pragma once

#include <unordered_map>

#include "../Core/Debug.h"
#include "../Graphics/DX11GPURasterizerState.h"

namespace aoe {

struct DX11RasterizerStateID {
	GPUCullMode cull_mode;
	GPUFillMode fill_mode;

	friend bool operator==(const DX11RasterizerStateID& lhs, const DX11RasterizerStateID& rhs) {
		return lhs.cull_mode == rhs.cull_mode 
			&& lhs.fill_mode == rhs.fill_mode;
	}
};

} // namespace aoe

namespace std {

template<>
struct hash<aoe::DX11RasterizerStateID> {
	size_t operator()(const aoe::DX11RasterizerStateID& value) const noexcept {
		size_t hash0 = hash<aoe::GPUCullMode>()(value.cull_mode);
		size_t hash1 = hash<aoe::GPUFillMode>()(value.fill_mode);
		return hash0 ^ hash1;
	}
};

} // namespace std

namespace aoe {

class DX11RasterizerStates {
public:
	DX11RasterizerStates() {
		CreateRasterizerState(GPUCullMode::kNone, GPUFillMode::kWireframe);
		CreateRasterizerState(GPUCullMode::kFront, GPUFillMode::kWireframe);
		CreateRasterizerState(GPUCullMode::kBack, GPUFillMode::kWireframe);
		CreateRasterizerState(GPUCullMode::kNone, GPUFillMode::kSolid);
		CreateRasterizerState(GPUCullMode::kFront, GPUFillMode::kSolid);
		CreateRasterizerState(GPUCullMode::kBack, GPUFillMode::kSolid);
	}

	const DX11GPURasterizerState& Get(GPUCullMode cull_mode, GPUFillMode fill_mode) const {
		DX11RasterizerStateID id{ cull_mode, fill_mode };
		return Get(id);
	}

	const DX11GPURasterizerState& Get(DX11RasterizerStateID id) const {
		auto it = rasterizer_states_.find(id);
		AOE_ASSERT_MSG(it != rasterizer_states_.end(), "There is no rasterizer state with providet options.");
		
		return it->second;
	}

private:
	std::unordered_map<DX11RasterizerStateID, DX11GPURasterizerState> rasterizer_states_;

	void CreateRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode) {
		DX11RasterizerStateID id{ cull_mode, fill_mode };
		GPURasterizerStateDescription desc{ cull_mode, fill_mode };
		rasterizer_states_.emplace(id, desc);
	}
};

} // namespace aoe