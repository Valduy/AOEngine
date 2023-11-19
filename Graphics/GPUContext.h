#pragma once

#include "DXHeaders.h"

namespace aoe {

class GPUContext{
	friend class GPUDevice;

public:
	void ClearState() {
		context_->ClearState();
	}

private:
	ID3D11DeviceContext* context_;

	GPUContext(ID3D11DeviceContext* context)
		: context_(context)
	{}
};

} // namespace aoe