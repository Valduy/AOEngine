#include "pch.h"

#include "../Renderer/DX11GeometryPassFrameSystem.h"
#include "../Renderer/DX11DirectionalLightPassFrameSystem.h"
#include "../Renderer/DX11PointLightPassFrameSystem.h"
#include "../Renderer/DX11DebugPassFrameSystem.h"
#include "../Renderer/DX11PreRenderPassTickSystem.h"
#include "../Renderer/DX11GeometryPassTickSystem.h"
#include "../Renderer/DX11PreLightPassTickSystem.h"
#include "../Renderer/DX11AmbientLightPassTickSystem.h"
#include "../Renderer/DX11DirectionalLightPassTickSystem.h"
#include "../Renderer/DX11PointLightPassTickSystem.h"
#include "../Renderer/DX11TonePassTickSystem.h"
#include "../Renderer/DX11DebugPassTickSystem.h"
#include "../Renderer/DX11PostRenderPassTickSystem.h"

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
	tick_systems_pool.PushSystem<DX11PreRenderPassTickSystem>();
	tick_systems_pool.PushSystem<DX11GeometryPassTickSystem>();
	tick_systems_pool.PushSystem<DX11PreLightPassTickSystem>();
	tick_systems_pool.PushSystem<DX11AmbientLightPassTickSystem>();
	tick_systems_pool.PushSystem<DX11DirectionalLightPassTickSystem>();
	tick_systems_pool.PushSystem<DX11PointLightPassTickSystem>();
	tick_systems_pool.PushSystem<DX11TonePassTickSystem>();
	tick_systems_pool.PushSystem<DX11DebugPassTickSystem>();
	tick_systems_pool.PushSystem<DX11PostRenderPassTickSystem>();
}

void SceneBase::InitializeFrameSystems(SystemsPool& frame_systems_pool) {
	frame_systems_pool.PushSystem<DX11GeometryPassFrameSystem>();
	frame_systems_pool.PushSystem<DX11DirectionalLightPassFrameSystem>();
	frame_systems_pool.PushSystem<DX11PointLightPassFrameSystem>();
	frame_systems_pool.PushSystem<DX11DebugPassFrameSystem>();
}

void SceneBase::SetupScene(ServiceProvider& service_provider) {

}

} // namespace aoe