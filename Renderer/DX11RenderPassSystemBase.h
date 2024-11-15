#pragma once

#include "../Game/ECSSystemBase.h"
#include "../Game/Relationeer.h"

#include "DX11RenderContext.h"
#include "CameraUtils.h"

namespace aoe {

class DX11RenderPassSystemBase : public ECSSystemBase {
public:
	DX11RenderPassSystemBase();

	DX11RenderContext* GetRenderContext();
	Relationeer<TransformComponent>* GetRelationeer();

	void Initialize(const aoe::ServiceProvider& service_provider) override;

protected:
	bool HasCamera();
	Entity GetActualCamera();
	Matrix4f GetCameraMatrix(Entity camera);

	Transform GetGlobalTransform(Entity entity);
	Vector3f GetGlobalPosition(Entity entity);
	Quaternion GetGlobalRotation(Entity entity);
	Matrix4f GetGlobalWorldMatrix(Entity entity);

private:
	DX11RenderContext* render_context_;
	Relationeer<TransformComponent>* relationeer_;
};

} // namespace aoe