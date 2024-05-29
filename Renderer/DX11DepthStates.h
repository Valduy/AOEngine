#pragma once

#include <unordered_map>

#include "../Core/Debug.h"
#include "../Graphics/DX11GPUDepthState.h"

namespace aoe {

struct DX11DepthStateID {
	bool is_depth_enabled;
	GPUDepthWriteMask write_mask;
	GPUComparsionFunction comparsion_function;

	friend bool operator==(const DX11DepthStateID& lhs, const DX11DepthStateID& rhs) {
		return lhs.is_depth_enabled == rhs.is_depth_enabled
			&& lhs.write_mask == rhs.write_mask
			&& lhs.comparsion_function == rhs.comparsion_function;
	}
};

} // namespace aoe

namespace std {

template<>
struct hash<aoe::DX11DepthStateID> {
	size_t operator()(const aoe::DX11DepthStateID& value) const noexcept {
		size_t hash0 = hash<bool>()(value.is_depth_enabled);
		size_t hash1 = hash<aoe::GPUDepthWriteMask>()(value.write_mask);
		size_t hash2 = hash<aoe::GPUComparsionFunction>()(value.comparsion_function);
		return hash0 ^ hash1 ^ hash2;
	}
};

} // namespace std

namespace aoe {

class DX11DepthStates {
public:
	DX11DepthStates();

	const DX11GPUDepthState& Get(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function) const;

	const DX11GPUDepthState& Get(DX11DepthStateID id) const;

private:
	std::unordered_map<DX11DepthStateID, DX11GPUDepthState> depth_states_;

	void CreateDepthStates();
	void CreateDepthStates(bool is_depth_enabled);
	void CreateDepthStates(bool is_depth_enabled, GPUDepthWriteMask write_mask);

	void CreateDepthState(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function);
};

} // namespace aoe