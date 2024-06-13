#pragma once

#include "DX11RenderPassBase.h"
#include "Line.h"

namespace aoe {

class DX11LineResourcesComponent {
public:
	DX11LineResourcesComponent(std::vector<DX11GPUBuffer> line_resources)
		: line_resources_(std::move(line_resources))
	{}

	const std::vector<DX11GPUBuffer>& GetLineResources() {
		return line_resources_;
	}

private:
	std::vector<DX11GPUBuffer> line_resources_;
};

class DX11DebugPass : public DX11RenderPassBase {
public:
	DX11DebugPass();

	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	
	void InitializeLineData();
	std::vector<DX11GPUBuffer> CreateLineResources(const Line& lines);
	DX11GPUBuffer CreateVertexBuffer(const Segment& segment);

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void UpdateLineData(Entity camera);
	void PrepareRenderContext();

	void OnTransformComponentAdded(Entity entity);
	void OnLineComponentAdded(Entity entity);

	void OnComponentRemoved(Entity entity);

	void SetupLineEntity(Entity entity);
};

} // namespace aoe