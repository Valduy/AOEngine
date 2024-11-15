#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11PostRenderPassTickSystem : public DX11RenderPassSystemBase {
public:
	void Update(float dt) override;

private:
	void PresentFrame();
	void ClearContext();
};

} // namespace aoe