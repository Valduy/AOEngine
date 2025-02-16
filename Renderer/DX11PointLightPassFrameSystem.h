#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11PointLightPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Update(float dt) override;

private:
	void UpdateRenderData();
};

} // namespace aoe