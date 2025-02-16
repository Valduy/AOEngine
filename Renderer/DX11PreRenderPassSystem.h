#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11PreRenderPassSystem : public DX11RenderPassSystemBase {
public:
	void Update(float dt) override;

private:
	void PrepareRenderContext();
	void UpdateCameras();
};

} // namespace aoe