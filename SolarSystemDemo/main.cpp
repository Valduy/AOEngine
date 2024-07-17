#include <functional>

#include "../Application/Application.h"
#include "../Core/Math.h"
#include "../ECS/World.h"
#include "../Resources/ModelLoader.h"
#include "../Game/TransformComponent.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformUtils.h"
#include "../Game/SystemsPool.h"
#include "../Renderer/DX11RenderSystem.h"
#include "../Renderer/AmbientLightComponent.h"
#include "../Renderer/DirectionalLightComponent.h"
#include "../Renderer/PointLightComponent.h"
#include "../Renderer/LineComponent.h"
#include "../Renderer/CameraComponent.h"
#include "../Renderer/Colors.h"
#include "../Renderer/DebugUtils.h"
#include "../Renderer/Material.h"
#include "../Renderer/RenderComponent.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"
#include "../Common/FlyCameraSystem.h"

class SolarSystemScene : public aoe::IScene {
public:
	SolarSystemScene(aoe::Application& application)
		: application_(application)
		, world_()
		, relationeer_(world_)
		, render_context_(application.GetWindow())
		, model_manager_()
		, texture_manager_()
		, service_provider_()
		, systems_pool_()
	{}

	void Initialize() override {
		aoe::ModelId dice_model_id = model_manager_.Load(L"/Content/Dice_d4.fbx", aoe::ModelLoaderOptions::kFlipUVs);
		aoe::TextureId dice_texture_id = texture_manager_.LoadRGBA(L"/Content/Dice_d4_Albedo.png");

		aoe::Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		aoe::Entity entity_dice_0 = world_.Create();
		world_.Add<aoe::TransformComponent>(entity_dice_0);
		world_.Add<aoe::RenderComponent>(entity_dice_0, dice_model_id, dice_texture_id, material);

		auto dice_0_transform_component = world_.Get<aoe::TransformComponent>(entity_dice_0);
		dice_0_transform_component->transform.position = { 0.0f, 0.0f, 4.0f };
		dice_0_transform_component->transform.scale = { 1.0f, 1.0f, 1.0f };

		auto other_material = material;
		other_material.diffuse = { 0.3, 1, 0.3 };

		aoe::Entity entity_dice_1 = world_.Create();
		world_.Add<aoe::TransformComponent>(entity_dice_1);
		world_.Add<aoe::RenderComponent>(entity_dice_1, dice_model_id, dice_texture_id, other_material);

		auto dice_1_transform_component = world_.Get<aoe::TransformComponent>(entity_dice_1);
		dice_1_transform_component->transform.position = { 1.0f, 1.0f, 1.0f };
		dice_1_transform_component->transform.rotation = aoe::Quaternion::FromEulerAngles(
			0.0f * aoe::Math::kDeg2Rad,
			0.0f * aoe::Math::kDeg2Rad,
			45.0f * aoe::Math::kDeg2Rad
		);
		dice_1_transform_component->transform.scale = { 1.0f, 1.0f, 1.0f };

		aoe::Entity entity_dice_2 = world_.Create();
		world_.Add<aoe::TransformComponent>(entity_dice_2);
		world_.Add<aoe::RenderComponent>(entity_dice_2, dice_model_id, dice_texture_id, other_material);

		auto dice_2_transform_component = world_.Get<aoe::TransformComponent>(entity_dice_2);
		dice_2_transform_component->transform.position = { 0.0f, -1.0f, 0.0f };
		dice_2_transform_component->transform.rotation = aoe::Quaternion::FromEulerAngles(
			90.0f * aoe::Math::kDeg2Rad,
			0.0f * aoe::Math::kDeg2Rad,
			0.0f * aoe::Math::kDeg2Rad
		);
		dice_2_transform_component->transform.scale = { 10.0f, 10.0f, 10û.0f };

		aoe::Transform axis_0_transform{};
		axis_0_transform.scale = { 3.0f, 3.0f, 3.0f };
		auto axis_0 = aoe::DebugUtils::CreateAxis(world_, relationeer_, axis_0_transform);
		relationeer_.SetParent(axis_0, entity_dice_0);

		aoe::Transform axis_1_transform{};
		axis_1_transform.scale = { 3.0f, 3.0f, 3.0f };
		auto axis_1 = aoe::DebugUtils::CreateAxis(world_, relationeer_, axis_1_transform);
		relationeer_.SetParent(axis_1, entity_dice_1);

		relationeer_.SetParent(entity_dice_0, entity_dice_1);

		aoe::TransformUtils::SetGlobalPosition(world_, relationeer_, entity_dice_0, { 0.0f, 0.0f, 0.0f });
		aoe::TransformUtils::SetGlobalRotation(world_, relationeer_, entity_dice_0, aoe::Quaternion::FromEulerAngles(
			0.0f * aoe::Math::kDeg2Rad,
			-45.0f * aoe::Math::kDeg2Rad,
			0.0f * aoe::Math::kDeg2Rad
		));

		aoe::Entity sphere = aoe::DebugUtils::CreateSphere(world_);
		relationeer_.SetParent(sphere, entity_dice_0);
		relationeer_.SetParent(aoe::DebugUtils::CreateAxis(world_, relationeer_), sphere);

		aoe::TransformUtils::SetGlobalPosition(world_, relationeer_, sphere, { 1.0f, 0.0f, -1.0f });
		aoe::TransformUtils::SetGlobalRotation(world_, relationeer_, sphere, aoe::Quaternion::FromEulerAngles(
			0.0f * aoe::Math::kDeg2Rad,
			45.0f * aoe::Math::kDeg2Rad,
			0.0f * aoe::Math::kDeg2Rad
		));

		aoe::DebugUtils::CreateGrid(world_, { 20, 0, 20 }, {}, aoe::Colors::kWhite);
		aoe::DebugUtils::CreateAxis(world_, relationeer_);

		aoe::Entity ambient_light = world_.Create();
		world_.Add<aoe::AmbientLightComponent>(ambient_light);
		auto ambient_light_component = world_.Get<aoe::AmbientLightComponent>(ambient_light);
		ambient_light_component->color = { 1.0f, 1.0f, 1.0f };
		ambient_light_component->intensity = 0.25f;

		aoe::Entity directional_light = world_.Create();
		world_.Add<aoe::TransformComponent>(directional_light);
		world_.Add<aoe::DirectionalLightComponent>(directional_light);
		auto direction_transform_component = world_.Get<aoe::TransformComponent>(directional_light);
		auto direction_light_component = world_.Get<aoe::DirectionalLightComponent>(directional_light);
		direction_light_component->color = { 1.0f, 1.0f, 1.0f };

		aoe::Entity point_light = world_.Create();
		world_.Add<aoe::TransformComponent>(point_light);
		world_.Add<aoe::PointLightComponent>(point_light);
		auto point_transform_component = world_.Get<aoe::TransformComponent>(point_light);
		auto point_light_component = world_.Get<aoe::PointLightComponent>(point_light);
		point_transform_component->transform.scale = { 5.0f, 5.0f, 5.0f };
		point_transform_component->transform.position = { 0.0f, 0.0f, 2.0f };
		point_light_component->color = { 1.0f, 1.0f, 1.0f };
		
		aoe::Entity point_light_sphere = aoe::DebugUtils::CreateSphere(world_, {}, aoe::Colors::kWhite);
		relationeer_.SetParent(point_light_sphere, point_light);

		aoe::Entity camera = world_.Create();
		world_.Add<aoe::TransformComponent>(camera);
		world_.Add<aoe::CameraComponent>(camera);
		world_.Add<aoe::FlyCameraComponent>(camera);
		
		auto camera_component = world_.Get<aoe::CameraComponent>(camera);
		camera_component->projection = aoe::Projection::kPerspective;
		camera_component->width = application_.GetWindow().GetWidth();
		camera_component->height = application_.GetWindow().GetHeight();
		camera_component->near_plain = 0.1f;
		camera_component->far_plain = 100.0f;
		camera_component->field_of_view = aoe::Math::kPi / 2;

		auto fly_camera_component = world_.Get<aoe::FlyCameraComponent>(camera);
		fly_camera_component->speed = 6.0f;
		fly_camera_component->sensitivity = 0.002f;

		service_provider_.AddService(&application_);
		service_provider_.AddService(&world_);
		service_provider_.AddService(&relationeer_);
		service_provider_.AddService(&render_context_);
		service_provider_.AddService(&model_manager_);
		service_provider_.AddService(&texture_manager_);

		systems_pool_.PushSystem<aoe::FlyCameraSystem>();
		systems_pool_.PushSystem<aoe::DX11RenderSystem>();
		systems_pool_.Initialize(service_provider_);
	};

	void Terminate() override {
		systems_pool_.Terminate();
	};

	void PerTickUpdate(float dt) override {
		systems_pool_.PerTickUpdate(dt);
		world_.Validate();
	}

	void PerFrameUpdate(float dt) override {
		systems_pool_.PerFrameUpdate(dt);
		world_.Validate();
	}

private:
	aoe::Application& application_;

	aoe::World world_;
	aoe::Relationeer<aoe::TransformComponent> relationeer_;

	aoe::DX11RenderContext render_context_;
	aoe::DX11ModelManager model_manager_;
	aoe::DX11TextureManager texture_manager_;

	aoe::ServiceProvider service_provider_;
	aoe::SystemsPool systems_pool_;
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	SolarSystemScene scene(application);
	application.Start(scene);

	return 0;
}