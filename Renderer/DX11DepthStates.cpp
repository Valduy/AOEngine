#include "pch.h"

#include "DX11DepthStates.h"

namespace aoe {

DX11DepthStates::DX11DepthStates() {
	CreateDepthStates();
}

const DX11GPUDepthState& DX11DepthStates::Get(
	bool is_depth_enabled, 
	GPUDepthWriteMask write_mask, 
	GPUComparsionFunction comparsion_function) const
{
	DX11DepthStateID id{ is_depth_enabled, write_mask, comparsion_function };
	return Get(id);
}

const DX11GPUDepthState& DX11DepthStates::Get(DX11DepthStateID id) const {
	auto it = depth_states_.find(id);
	AOE_ASSERT_MSG(it != depth_states_.end(), "There is no depth state with providet options.");

	return it->second;
}

void DX11DepthStates::CreateDepthStates() {
	CreateDepthStates(false);
	CreateDepthStates(true);
}

void DX11DepthStates::CreateDepthStates(bool is_depth_enabled) {
	CreateDepthStates(is_depth_enabled, GPUDepthWriteMask::kWriteNone);
	CreateDepthStates(is_depth_enabled, GPUDepthWriteMask::kWriteAll);
}

void DX11DepthStates::CreateDepthStates(bool is_depth_enabled, GPUDepthWriteMask write_mask) {
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kNever);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kLess);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kEqual);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kLessOrEqual);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kGreater);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kNotEqual);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kGreaterOrEqual);
	CreateDepthState(is_depth_enabled, write_mask, GPUComparsionFunction::kAlways);
}

void DX11DepthStates::CreateDepthState(
	bool is_depth_enabled, 
	GPUDepthWriteMask write_mask, 
	GPUComparsionFunction comparsion_function)
{
	DX11DepthStateID id{ is_depth_enabled, write_mask, comparsion_function };
	GPUDepthStateDescription desc{ is_depth_enabled, write_mask, comparsion_function };
	depth_states_.emplace(id, desc);
}

} // namespace aoe