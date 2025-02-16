#pragma once

#include "../Graphics/DX11GPUBuffer.h"

#include "DX11RenderPassSystemBase.h"
#include "LineComponent.h"

namespace aoe {

class DX11DebugPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Update(float dt) override;

private:
	void UpdateRenderData();
};

} // namespace aoe