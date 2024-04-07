#pragma once

#include "../Application/Window.h"
#include "../Game/IECSSystem.h"
#include "../Game/ServiceProvider.h"
#include "../Game/TransformComponent.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformUtils.h"
#include "../Graphics/DX11GPUContext.h"
#include "../ECS/World.h"

#include "RenderComponent.h"
#include "DX11RenderDataComponent.h"

namespace aoe {

class DX11RenderDataUpdateSystem : public IECSSystem {
public:
	DX11RenderDataUpdateSystem(const Window& window, const aoe::ServiceProvider& service_provider)
		: window_(window)
		, service_provider_(service_provider)
		, world_(nullptr)
		, relationeer_(nullptr)
	{}

	void Initialize() override {
		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		relationeer_ = service_provider_.GetService<Relationeer<TransformComponent>>();
		AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");

		// TODO: camera service;

		InitializeRenderData();
		SubscribeToComponents();
	}

	void Terminate() override {
		UnsibscribeFromComponents();
	}

	void PerTickUpdate(float dt) override {}

	void PerFrameUpdate(float dt) override {
		const float width = static_cast<float>(window_.GetWidth());
		const float height = static_cast<float>(window_.GetHeight());
		const float aspect = width / height;
		const float fov = aoe::Math::kPi / 3;
		const float near_plain = 0.1f;
		const float far_plain = 100.0f;
		const float handedness = -1.0f;

		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		Matrix4 view_matrix = Matrix4::LookAt(Math::kV3AxisZ, Math::kV3Zero, Math::kV3AxisY, handedness);
		Matrix4 projection_matrix = Matrix4::Perspective(fov, aspect, near_plain, far_plain, handedness);
		Matrix4 camera_matrix = projection_matrix * view_matrix;

		world_->ForEach<TransformComponent, DX11RenderDataComponent>([&, *this](auto entity, auto transform_component, auto render_data_component) {
			Matrix4 world = TransformUtils::GetGlobalWorldMatrix(*world_, *relationeer_, entity);
			Matrix4 world_view_projection = camera_matrix * world;
			
			TransformData data{};
			data.world = world.Transpose();
			data.world_view_projection = world_view_projection.Transpose();
			data.inverse_transpose_world = world.Transpose().Inverse().Transpose();

			context.UpdateBuffer<TransformData>(render_data_component->transform_data_buffer, &data, 1);
		});
	}

private:
	const Window& window_;
	const ServiceProvider& service_provider_;

	World* world_;
	Relationeer<TransformComponent>* relationeer_;

	void InitializeRenderData() {
		world_->ForEach<TransformComponent, RenderComponent>([this](auto entity, auto transform_component, auto render_component) {
			world_->Add<DX11RenderDataComponent>(entity);
		});
	}

	void SubscribeToComponents() {
		world_->ComponentAdded<TransformComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentAdded);
		world_->ComponentAdded<RenderComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentAdded);
		world_->ComponentRemoved<TransformComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentRemoved);
		world_->ComponentRemoved<RenderComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentRemoved);
	}

	void UnsibscribeFromComponents() {
		world_->ComponentAdded<TransformComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentAdded);
		world_->ComponentAdded<RenderComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentAdded);
		world_->ComponentRemoved<TransformComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentRemoved);
		world_->ComponentRemoved<RenderComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentRemoved);
	}

	void OnTransformComponentAdded(Entity entity) {
		if (world_->Has<RenderComponent>(entity) && !world_->Has<DX11RenderDataComponent>(entity)) {
			world_->Add<DX11RenderDataComponent>(entity);
		}
	}

	void OnRenderComponentAdded(Entity entity) {
		if (world_->Has<TransformComponent>(entity) && !world_->Has<DX11RenderDataComponent>(entity)) {
			world_->Add<DX11RenderDataComponent>(entity);
		}
	}

	void OnTransformComponentRemoved(Entity entity) {
		world_->Remove<DX11RenderDataComponent>(entity);
	}

	void OnRenderComponentRemoved(Entity entity) {
		world_->Remove<DX11RenderDataComponent>(entity);
	}
};

} // namespace aoe