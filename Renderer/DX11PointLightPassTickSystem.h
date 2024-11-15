#pragma once

#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"
#include "../Resources/DX11ModelManager.h"

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11PointLightPassTickSystem : public DX11RenderPassSystemBase {
public:
	DX11PointLightPassTickSystem();

	void Initialize(const ServiceProvider& service_provider) override;
	void Update(float dt) override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;

	DX11ModelManager* model_manager_;
	ModelId sphere_id;

	void Render();
	void PrepareRenderContext();
};

} // namespace aoe