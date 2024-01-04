#pragma once

#include "../Core/ClassHelper.h"

#include "DXHelper.h"
#include "IGPUResource.h"

namespace aoe {

class GPUContext;

class GPUDevice : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUDevice)
	
	ID3D11Device* GetNative() const;
	GPUContext GetContext() const;

	GPUDevice();

	bool Initialize();
	void Terminate() override;

private:
	ID3D11Device* device_;
	ID3D11DeviceContext* context_;
};

} // namespace aoe