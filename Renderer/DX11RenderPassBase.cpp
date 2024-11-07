#include "pch.h"

#include "DX11RenderPassBase.h"

namespace aoe {

DX11RenderPassBase::DX11RenderPassBase()
	: world_(nullptr)
	, render_context_(nullptr)
	, relationeer_(nullptr)
{}

World* DX11RenderPassBase::GetWorld() {
	return world_;
}

DX11RenderContext* DX11RenderPassBase::GetRenderContext() {
	return render_context_;
}

Relationeer<TransformComponent>* DX11RenderPassBase::GetRelationeer() {
	return relationeer_;
}

void DX11RenderPassBase::Initialize(const aoe::ServiceProvider& service_provider) {
	world_ = service_provider.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

	render_context_ = service_provider.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

	relationeer_ = service_provider.GetService<Relationeer<TransformComponent>>();
	AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");
}

Entity DX11RenderPassBase::GetActualCamera() {
	return CameraUtils::GetActualCamera(*world_);
}

Transform DX11RenderPassBase::GetGlobalTransform(Entity entity) {
	return TransformUtils::GetGlobalTransform(*world_, *relationeer_, entity);
}

Vector3f DX11RenderPassBase::GetGlobalPosition(Entity entity) {
	return TransformUtils::GetGlobalPosition(*world_, *relationeer_, entity);
}

Quaternion DX11RenderPassBase::GetGlobalRotation(Entity entity) {
	return TransformUtils::GetGlobalRotation(*world_, *relationeer_, entity);
}

Matrix4f DX11RenderPassBase::GetGlobalWorldMatrix(Entity entity) {
	return TransformUtils::GetGlobalWorldMatrix(*world_, *relationeer_, entity);
}

Matrix4f DX11RenderPassBase::GetCameraMatrix(Entity camera) {
	return CameraUtils::GetCameraMatrix(*world_, *relationeer_, camera);
}

} // namespace aoe