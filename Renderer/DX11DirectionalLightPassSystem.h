#pragma once

#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"

#include "DX11RenderPassSystemBase.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"

namespace aoe {

class DX11DirectionalLightPassSystem : public DX11RenderPassSystemBase {
public:
	DX11DirectionalLightPassSystem();

	void Update(float dt) override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11RenderData<Vector3fData> camera_data_;

	void UpdateRenderData();
	void Render(Entity camera);
	void PrepareRenderContext();
};

} // namespace aoe