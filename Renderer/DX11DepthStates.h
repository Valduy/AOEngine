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
	DX11DepthStates() {
		CreateDepthStates();
	}

	const DX11GPUDepthState& Get(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function) const
	{
		DX11DepthStateID id{ is_depth_enabled, write_mask, comparsion_function };
		return Get(id);
	}

	const DX11GPUDepthState& Get(DX11DepthStateID id) const
	{
		auto it = depth_states_.find(id);
		AOE_ASSERT_MSG(it != depth_states_.end(), "There is no depth state with providet options.");
		
		return it->second;
	}

private:
	std::unordered_map<DX11DepthStateID, DX11GPUDepthState> depth_states_;

	void CreateDepthStates() {
		CreateDepthStates(false);
		CreateDepthStates(true);
	}

	void CreateDepthStates(bool is_depth_enabled) {
		CreateDepthStates(is_depth_enabled, GPUDepthWriteMask::kWriteNone);
		CreateDepthStates(is_depth_enabled, GPUDepthWriteMask::kWriteAll);
	}

	void CreateDepthStates(bool is_depth_enabled, GPUDepthWriteMask write_mask) {
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kNever);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kLess);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kEqual);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kLessOrEqual);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kGreater);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kNotEqual);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kGreaterOrEqual);
		CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kAlways);
	}

	void CreateDepthState(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function)
	{
		DX11DepthStateID id{ is_depth_enabled, write_mask, comparsion_function };
		GPUDepthStateDescription desc{ is_depth_enabled, write_mask, comparsion_function };
		depth_states_.emplace(id, desc);
	}
};

} // namespace aoe