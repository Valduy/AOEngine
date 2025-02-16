#pragma once

#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"
#include "../Resources/DX11ModelManager.h"

#include "DX11RenderPassSystemBase.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"

namespace aoe {

class DX11PointLightPassSystem : public DX11RenderPassSystemBase {
public:
	DX11PointLightPassSystem();

	void Initialize(const ServiceProvider& service_provider) override;
	void Update(float dt) override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11RenderData<PointLightCameraData> camera_data_;

	DX11ModelManager* model_manager_;
	ModelId sphere_id;

	void UpdateRenderData();
	void Render(Entity camera);
	void PrepareRenderContext();
};

} // namespace aoe