#pragma once

#include "../ECS/World.h"
#include "../Game/ServiceProvider.h"
#include "../Game/Relationeer.h"
#include "../Graphics/DX11GPUContext.h"

#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"
#include "CameraUtils.h"

namespace aoe {

class DX11RenderPassBase {
public:
	DX11RenderPassBase();
	virtual ~DX11RenderPassBase() = default;

	World* GetWorld();
	DX11RenderContext* GetRenderContext();
	Relationeer<TransformComponent>* GetRelationeer();

	virtual void Initialize(const aoe::ServiceProvider& service_provider);
	virtual void Terminate() {}
	virtual void Update() {}
	virtual void Render() {}

protected:
	Entity GetActualCamera();
	Transform GetGlobalTransform(Entity entity);
	Vector3f GetGlobalPosition(Entity entity);
	Quaternion GetGlobalRotation(Entity entity);
	Matrix4f GetGlobalWorldMatrix(Entity entity);
	Matrix4f GetCameraMatrix(Entity camera);

private:
	World* world_;
	DX11RenderContext* render_context_;
	Relationeer<TransformComponent>* relationeer_;
};

} // namespace aoe