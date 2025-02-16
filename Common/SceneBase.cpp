#include "pch.h"

#include "../Renderer/DX11PreRenderPassSystem.h"
#include "../Renderer/DX11GeometryPassSystem.h"
#include "../Renderer/DX11PreLightPassSystem.h"
#include "../Renderer/DX11AmbientLightPassSystem.h"
#include "../Renderer/DX11DirectionalLightPassSystem.h"
#include "../Renderer/DX11PointLightPassSystem.h"
#include "../Renderer/DX11TonePassSystem.h"
#include "../Renderer/DX11DebugPassSystem.h"
#include "../Renderer/DX11PostRenderPassSystem.h"

#include "SceneBase.h"

namespace aoe {

SceneBase::SceneBase(Application& application)
	: application_(application)
	, world_()
	, relationeer_(world_)
	, render_context_(application.GetWindow())
	, model_manager_()
	, texture_manager_()
	, service_provider_()
	, tick_systems_pool_()
	, frame_systems_pool_()
{}

void SceneBase::Initialize() {
	SetupServices(service_provider_);

	InitializeTickSystems(tick_systems_pool_);
	tick_systems_pool_.Initialize(service_provider_);

	InitializeFrameSystems(frame_systems_pool_);
	frame_systems_pool_.Initialize(service_provider_);

	SetupScene(service_provider_);
}

void SceneBase::Terminate() {
	frame_systems_pool_.Terminate();
	tick_systems_pool_.Terminate();
}

void SceneBase::PerTickUpdate(float dt) {
	tick_systems_pool_.Update(dt);
	world_.Validate();
}

void SceneBase::PerFrameUpdate(float dt) {
	frame_systems_pool_.Update(dt);
	world_.Validate();
}

void SceneBase::SetupServices(ServiceProvider& service_provider) {
	service_provider.AddService(&application_);
	service_provider.AddService(&world_);
	service_provider.AddService(&relationeer_);
	service_provider.AddService(&render_context_);
	service_provider.AddService(&model_manager_);
	service_provider.AddService(&texture_manager_);
}

void SceneBase::InitializeTickSystems(SystemsPool& tick_systems_pool) {
	tick_systems_pool.PushSystem<DX11PreRenderPassSystem>();
	tick_systems_pool.PushSystem<DX11GeometryPassSystem>();
	tick_systems_pool.PushSystem<DX11PreLightPassSystem>();
	tick_systems_pool.PushSystem<DX11AmbientLightPassSystem>();
	tick_systems_pool.PushSystem<DX11DirectionalLightPassSystem>();
	tick_systems_pool.PushSystem<DX11PointLightPassSystem>();
	tick_systems_pool.PushSystem<DX11TonePassSystem>();
	tick_systems_pool.PushSystem<DX11DebugPassSystem>();
	tick_systems_pool.PushSystem<DX11PostRenderPassSystem>();
}

void SceneBase::InitializeFrameSystems(SystemsPool& frame_systems_pool) {}

void SceneBase::SetupScene(ServiceProvider& service_provider) {}

} // namespace aoe