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
#include "AmbientLightComponent.h"
#include "DirectionLightComponent.h"
#include "DX11RenderDataComponents.h"
#include "CameraUtils.h"

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

		InitializeGeometryData();
		InitializeAmbientLightData();
		InitializeDirectionLightData();

		SubscribeToComponents();
	}

	void Terminate() override {
		UnsibscribeFromComponents();
	}

	void PerTickUpdate(float dt) override {}

	void PerFrameUpdate(float dt) override {
		Entity camera = CameraUtils::GetActualCamera(*world_);

		if (camera.IsNull()) {
			return;
		}

		UpdateGeometryData(camera);
		UpdateAmbientLightData();
		UpdateDirectionLightData(camera);
	}

private:
	const Window& window_;
	const ServiceProvider& service_provider_;

	World* world_;
	Relationeer<TransformComponent>* relationeer_;

	void InitializeGeometryData() {
		world_->ForEach<TransformComponent, RenderComponent>(
		[this](auto entity, auto transform_component, auto render_component) {
			world_->Add<DX11TransformDataComponent>(entity);
			world_->Add<DX11MaterialDataComponent>(entity);
		});
	}

	void InitializeAmbientLightData() {
		world_->ForEach<AmbientLightComponent>(
		[this](auto entity, auto ambient_light_component) {
			world_->Add<DX11AmbientLightDataComponent>(entity);
		});
	}

	void InitializeDirectionLightData() {
		world_->ForEach<DirectionLightComponent>(
		[this](auto entity, auto ambient_light_component) {
			world_->Add<DX11DirectionLightDataComponent>(entity);
		});
	}

	void SubscribeToComponents() {
		world_->ComponentAdded<TransformComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentAdded);
		world_->ComponentAdded<RenderComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentAdded);
		world_->ComponentAdded<AmbientLightComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnAmbientLightComponentAdded);
		world_->ComponentAdded<DirectionLightComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnDirectionLightComponentAdded);

		world_->ComponentRemoved<TransformComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentRemoved);
		world_->ComponentRemoved<RenderComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentRemoved);	
		world_->ComponentRemoved<AmbientLightComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnAmbientLightComponentRemoved);
		world_->ComponentRemoved<DirectionLightComponent>().Attach(*this, &DX11RenderDataUpdateSystem::OnDirectionLightComponentRemoved);
	}

	void UnsibscribeFromComponents() {
		world_->ComponentAdded<TransformComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentAdded);
		world_->ComponentAdded<RenderComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentAdded);
		world_->ComponentAdded<AmbientLightComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnAmbientLightComponentAdded);
		world_->ComponentAdded<DirectionLightComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnDirectionLightComponentAdded);

		world_->ComponentRemoved<TransformComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnTransformComponentRemoved);
		world_->ComponentRemoved<RenderComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnRenderComponentRemoved);
		world_->ComponentRemoved<AmbientLightComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnAmbientLightComponentRemoved);
		world_->ComponentRemoved<DirectionLightComponent>().Detach(*this, &DX11RenderDataUpdateSystem::OnDirectionLightComponentRemoved);
	}

	void OnTransformComponentAdded(Entity entity) {
		if (world_->Has<RenderComponent>(entity) && !world_->Has<DX11TransformDataComponent>(entity)) {
			world_->Add<DX11TransformDataComponent>(entity);
		}
	}

	void OnRenderComponentAdded(Entity entity) {
		if (world_->Has<TransformComponent>(entity) && !world_->Has<DX11TransformDataComponent>(entity)) {
			world_->Add<DX11TransformDataComponent>(entity);
		}

		world_->Add<DX11MaterialDataComponent>(entity);
	}

	void OnAmbientLightComponentAdded(Entity entity) {
		world_->Add<DX11AmbientLightDataComponent>(entity);
	}

	void OnDirectionLightComponentAdded(Entity entity) {
		world_->Add<DX11DirectionLightDataComponent>(entity);
	}

	void OnTransformComponentRemoved(Entity entity) {
		world_->Remove<DX11TransformDataComponent>(entity);
	}

	void OnRenderComponentRemoved(Entity entity) {
		world_->Remove<DX11TransformDataComponent>(entity);
		world_->Remove<DX11MaterialDataComponent>(entity);
	}

	void OnAmbientLightComponentRemoved(Entity entity) {
		world_->Remove<DX11AmbientLightDataComponent>(entity);
	}

	void OnDirectionLightComponentRemoved(Entity entity) {
		world_->Remove<DX11DirectionLightDataComponent>(entity);
	}

	void UpdateGeometryData(Entity camera) {
		Matrix4 camera_matrix = CameraUtils::GetCameraMatrix(*world_, camera);

		world_->ForEach<TransformComponent, RenderComponent, DX11TransformDataComponent, DX11MaterialDataComponent>(
		[&, this](auto entity, auto transform_component, auto render_component, auto transform_data_component, auto render_data_component) {
			Matrix4 world = TransformUtils::GetGlobalWorldMatrix(*world_, *relationeer_, entity);
			Matrix4 world_view_projection = camera_matrix * world;

			TransformData transform_data{};
			transform_data.world = world.Transpose();
			transform_data.world_view_projection = world_view_projection.Transpose();
			transform_data.inverse_transpose_world = world.Inverse();

			MaterialData material_data{};
			material_data.diffuse = render_component->material.diffuse;
			material_data.specular = render_component->material.specular;
			material_data.shininess = render_component->material.shininess;

			transform_data_component->Update(&transform_data);
			render_data_component->Update(&material_data);
		});
	}

	void UpdateAmbientLightData() {
		world_->ForEach<AmbientLightComponent, DX11AmbientLightDataComponent>(
		[this](auto entity, auto ambient_light_component, auto ambient_light_data_component) {
			AmbientLightData data{};
			data.color = ambient_light_component->color;
			data.intensity = ambient_light_component->intensity;

			ambient_light_data_component->Update(&data);
		});
	}

	void UpdateDirectionLightData(Entity camera) {
		auto transform_component = world_->Get<TransformComponent>(camera);

		world_->ForEach<TransformComponent, DirectionLightComponent, DX11DirectionLightDataComponent>(
		[&, this](auto entity, auto transform_component, auto direction_light_component, auto direction_light_data_component) {
			DirectionLightData data{};
			data.view_position = transform_component->position;
			data.direction = TransformUtils::GetGlobalForward(*world_, *relationeer_, entity);
			data.color = direction_light_component->color;

			direction_light_data_component->Update(&data);
		});
	}
};

} // namespace aoe