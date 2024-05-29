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
	DX11RasterizerStates();

	const DX11GPURasterizerState& Get(GPUCullMode cull_mode, GPUFillMode fill_mode) const;
	const DX11GPURasterizerState& Get(DX11RasterizerStateID id) const;

private:
	std::unordered_map<DX11RasterizerStateID, DX11GPURasterizerState> rasterizer_states_;

	void CreateRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode);
};

} // namespace aoe