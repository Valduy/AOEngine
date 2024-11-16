#include "../Game/TransformUtils.h"
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
#include "../Common/SceneBase.h"
#include "../Common/FlyCameraTickSystem.h"
#include "../Common/FlyCameraFrameSystem.h"

class SolarSystemScene : public aoe::SceneBase {
public:
	SolarSystemScene(aoe::Application& application)
		: aoe::SceneBase(application)
	{}

protected:
	void InitializeTickSystems(aoe::SystemsPool& tick_systems_pool) override {
		tick_systems_pool.PushSystem<aoe::FlyCameraTickSystem>();

		aoe::SceneBase::InitializeTickSystems(tick_systems_pool);
	}

	void InitializeFrameSystems(aoe::SystemsPool& frame_systems_pool) override {
		frame_systems_pool.PushSystem<aoe::FlyCameraFrameSystem>();

		aoe::SceneBase::InitializeFrameSystems(frame_systems_pool);
	}

	void SetupScene(aoe::ServiceProvider& service_provider) override {
		using namespace aoe;

		Application& application = service_provider.GetService<Application>();
		World& world = service_provider.GetService<World>();
		Relationeer<TransformComponent>& relationeer = service_provider.GetService<Relationeer<TransformComponent>>();
		DX11ModelManager& model_manager = service_provider.GetService<DX11ModelManager>();
		DX11TextureManager& texture_manager = service_provider.GetService<DX11TextureManager>();

		ModelId dice_model_id = model_manager.Load(L"/Content/Dice_d4.fbx", ModelLoaderOptions::kFlipUVs);
		TextureId dice_texture_id = texture_manager.LoadRGBA(L"/Content/Dice_d4_Albedo.png");

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		Entity entity_dice_0 = world.CreateEntity();
		world.AddComponent<TransformComponent>(entity_dice_0);
		world.AddComponent<RenderComponent>(entity_dice_0, dice_model_id, dice_texture_id, material);

		auto dice_0_transform_component = world.GetComponent<TransformComponent>(entity_dice_0);
		dice_0_transform_component->transform.position = { 0.0f, 0.0f, 4.0f };
		dice_0_transform_component->transform.scale = { 1.0f, 1.0f, 1.0f };

		auto other_material = material;
		other_material.diffuse = { 0.3f, 1.0f, 0.3f };

		Entity entity_dice_1 = world.CreateEntity();
		world.AddComponent<TransformComponent>(entity_dice_1);
		world.AddComponent<RenderComponent>(entity_dice_1, dice_model_id, dice_texture_id, other_material);

		auto dice_1_transform_component = world.GetComponent<TransformComponent>(entity_dice_1);
		dice_1_transform_component->transform.position = { 1.0f, 1.0f, 1.0f };
		dice_1_transform_component->transform.rotation = Quaternion::FromEulerAngles(
			0.0f * Math::kDeg2Rad,
			0.0f * Math::kDeg2Rad,
			45.0f * Math::kDeg2Rad
		);
		dice_1_transform_component->transform.scale = { 1.0f, 1.0f, 1.0f };

		Entity entity_dice_2 = world.CreateEntity();
		world.AddComponent<TransformComponent>(entity_dice_2);
		world.AddComponent<RenderComponent>(entity_dice_2, dice_model_id, dice_texture_id, other_material);

		auto dice_2_transform_component = world.GetComponent<TransformComponent>(entity_dice_2);
		dice_2_transform_component->transform.position = { 0.0f, -1.0f, 0.0f };
		dice_2_transform_component->transform.rotation = Quaternion::FromEulerAngles(
			90.0f * Math::kDeg2Rad,
			0.0f * Math::kDeg2Rad,
			0.0f * Math::kDeg2Rad
		);
		dice_2_transform_component->transform.scale = { 10.0f, 10.0f, 10.0f };

		Transform axis_0_transform{};
		axis_0_transform.scale = { 3.0f, 3.0f, 3.0f };
		Entity axis_0 = DebugUtils::CreateAxis(world, relationeer, axis_0_transform);
		relationeer.SetParent(axis_0, entity_dice_0);

		Transform axis_1_transform{};
		axis_1_transform.scale = { 3.0f, 3.0f, 3.0f };
		Entity axis_1 = DebugUtils::CreateAxis(world, relationeer, axis_1_transform);
		relationeer.SetParent(axis_1, entity_dice_1);

		relationeer.SetParent(entity_dice_0, entity_dice_1);

		TransformUtils::SetGlobalPosition(world, relationeer, entity_dice_0, { 0.0f, 0.0f, 0.0f });
		TransformUtils::SetGlobalRotation(world, relationeer, entity_dice_0, Quaternion::FromEulerAngles(
			0.0f * Math::kDeg2Rad,
			-45.0f * Math::kDeg2Rad,
			0.0f * Math::kDeg2Rad
		));

		Entity sphere = DebugUtils::CreateSphere(world);
		relationeer.SetParent(sphere, entity_dice_0);
		relationeer.SetParent(DebugUtils::CreateAxis(world, relationeer), sphere);

		TransformUtils::SetGlobalPosition(world, relationeer, sphere, { 1.0f, 0.0f, -1.0f });
		TransformUtils::SetGlobalRotation(world, relationeer, sphere, Quaternion::FromEulerAngles(
			0.0f * Math::kDeg2Rad,
			45.0f * Math::kDeg2Rad,
			0.0f * Math::kDeg2Rad
		));

		DebugUtils::CreateGrid(world, { 20, 0, 20 }, {}, Colors::kWhite);
		DebugUtils::CreateAxis(world, relationeer);

		Entity ambient_light = world.CreateEntity();
		world.AddComponent<AmbientLightComponent>(ambient_light);
		auto ambient_light_component = world.GetComponent<AmbientLightComponent>(ambient_light);
		ambient_light_component->color = { 1.0f, 1.0f, 1.0f };
		ambient_light_component->intensity = 0.25f;

		Entity directional_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(directional_light);
		world.AddComponent<DirectionalLightComponent>(directional_light);
		auto direction_transform_component = world.GetComponent<TransformComponent>(directional_light);
		auto direction_light_component = world.GetComponent<DirectionalLightComponent>(directional_light);
		direction_light_component->color = { 1.0f, 1.0f, 1.0f };

		Entity point_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(point_light);
		world.AddComponent<PointLightComponent>(point_light);
		auto point_transform_component = world.GetComponent<TransformComponent>(point_light);
		auto point_light_component = world.GetComponent<PointLightComponent>(point_light);
		point_transform_component->transform.scale = { 5.0f, 5.0f, 5.0f };
		point_transform_component->transform.position = { 0.0f, 0.0f, 2.0f };
		point_light_component->color = { 1.0f, 1.0f, 1.0f };

		Entity point_light_sphere = DebugUtils::CreateSphere(world, {}, Colors::kWhite);
		relationeer.SetParent(point_light_sphere, point_light);

		Entity camera = world.CreateEntity();
		world.AddComponent<TransformComponent>(camera);
		world.AddComponent<CameraComponent>(camera);
		world.AddComponent<FlyCameraComponent>(camera);

		auto camera_component = world.GetComponent<CameraComponent>(camera);
		camera_component->projection = Projection::kPerspective;
		camera_component->width = application.GetWindow().GetWidth();
		camera_component->height = application.GetWindow().GetHeight();
		camera_component->near_plain = 0.1f;
		camera_component->far_plain = 100.0f;
		camera_component->field_of_view = Math::kPi / 2;

		auto fly_camera_component = world.GetComponent<FlyCameraComponent>(camera);
		fly_camera_component->speed = 6.0f;
		fly_camera_component->sensitivity = 0.002f;
	}
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	SolarSystemScene scene(application);
	application.Start(scene);

	return 0;
}