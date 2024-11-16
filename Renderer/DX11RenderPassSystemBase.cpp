#include "pch.h"

#include "DX11RenderPassSystemBase.h"

namespace aoe {

DX11RenderPassSystemBase::DX11RenderPassSystemBase()
	: render_context_(nullptr)
	, relationeer_(nullptr)
{}

DX11RenderContext* DX11RenderPassSystemBase::GetRenderContext() {
	return render_context_;
}

Relationeer<TransformComponent>* DX11RenderPassSystemBase::GetRelationeer() {
	return relationeer_;
}

void DX11RenderPassSystemBase::Initialize(const aoe::ServiceProvider& service_provider) {
	ECSSystemBase::Initialize(service_provider);

	render_context_ = service_provider.TryGetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

	relationeer_ = service_provider.TryGetService<Relationeer<TransformComponent>>();
	AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");
}

bool DX11RenderPassSystemBase::HasCamera() {
	return CameraUtils::HasCamera(*GetWorld());
}

Entity DX11RenderPassSystemBase::GetActualCamera() {
	return CameraUtils::GetActualCamera(*GetWorld());
}

Matrix4f DX11RenderPassSystemBase::GetCameraMatrix(Entity camera) {
	return CameraUtils::GetCameraMatrix(*GetWorld(), *relationeer_, camera);
}

Transform DX11RenderPassSystemBase::GetGlobalTransform(Entity entity) {
	return TransformUtils::GetGlobalTransform(*GetWorld(), *relationeer_, entity);
}

Vector3f DX11RenderPassSystemBase::GetGlobalPosition(Entity entity) {
	return TransformUtils::GetGlobalPosition(*GetWorld(), *relationeer_, entity);
}

Quaternion DX11RenderPassSystemBase::GetGlobalRotation(Entity entity) {
	return TransformUtils::GetGlobalRotation(*GetWorld(), *relationeer_, entity);
}

Matrix4f DX11RenderPassSystemBase::GetGlobalWorldMatrix(Entity entity) {
	return TransformUtils::GetGlobalWorldMatrix(*GetWorld(), *relationeer_, entity);
}

} // namespace aoe