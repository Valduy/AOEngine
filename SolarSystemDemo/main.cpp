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

#include "RotationSystem.h"

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
		frame_systems_pool.PushSystem<RotationSystem>();

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

		Entity sun = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Glowstone.png");
		auto sun_transform = world.GetComponent<TransformComponent>(sun);
		sun_transform->transform.scale = { 2.0, 2.0, 2.0 };
		world.AddComponent<RotationComponent>(sun, Math::kUp, 0.4f);

		Entity sun_light = CreatePointLight(world, relationeer);
		auto sun_light_transform = world.GetComponent<TransformComponent>(sun_light);
		sun_light_transform->transform.scale = { 5.0f, 5.0f, 5.0f };

		relationeer.SetParent(sun_light, sun);

		Entity cobblestone = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Cobblestone.png");
		auto cobblestone_transform = world.GetComponent<TransformComponent>(cobblestone);
		cobblestone_transform->transform.position = { 0.0f, 0.0f, 2.0f };
		cobblestone_transform->transform.scale = { 0.6f, 0.6f, 0.6f };
		world.AddComponent<RotationComponent>(cobblestone, Math::kUp, 0.6f);

		relationeer.SetParent(cobblestone, sun);

		Entity dirt = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Dirt.png");
		auto dirt_transform = world.GetComponent<TransformComponent>(dirt);
		dirt_transform->transform.position = { 2.5f, 0.0f, 2.5f };
		dirt_transform->transform.scale = { 0.5f, 0.5f, 0.5f };
		dirt_transform->transform.rotation = Quaternion::FromAngleAxis(30.0f * Math::kDeg2Rad, Math::kRight);
		world.AddComponent<RotationComponent>(dirt, dirt_transform->transform.GetUp(), 1.0f);

		relationeer.SetParent(dirt, sun);

		Entity endstone = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Endstone.png");
		auto endstone_transform = world.GetComponent<TransformComponent>(endstone);
		endstone_transform->transform.position = { 0.0f, 0.0f, 1.5f };
		endstone_transform->transform.scale = { 0.6f, 0.6f, 0.6f };
		world.AddComponent<RotationComponent>(endstone, Math::kUp, 1.5f);

		relationeer.SetParent(endstone, dirt);

		Entity sand = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Sand.png");
		auto sand_transform = world.GetComponent<TransformComponent>(sand);
		sand_transform->transform.position = { -3.0f, 0.0f, -3.0f };
		sand_transform->transform.scale = { 0.7f, 0.7f, 0.7f };
		sand_transform->transform.rotation = Quaternion::FromAngleAxis(30.0f * Math::kDeg2Rad, Math::kForward);
		world.AddComponent<RotationComponent>(sand, sand_transform->transform.GetUp(), 0.8f);

		relationeer.SetParent(sand, sun);

		Entity hellstone = CreateAstroObject(world, relationeer, model_manager, texture_manager, L"/Content/Hellstone.png");
		auto hellston_transform = world.GetComponent<TransformComponent>(hellstone);
		hellston_transform->transform.position = { 1.2f, 0.0f, 0.0f };
		hellston_transform->transform.scale = { 0.4f, 0.4f, 0.4f };
		world.AddComponent<RotationComponent>(hellstone, Math::kUp, 2.0f);

		relationeer.SetParent(hellstone, sand);

		DebugUtils::CreateGrid(world, { 20, 0, 20 }, {}, Colors::kWhite);
		DebugUtils::CreateAxis(world, relationeer);

		CreateAmbientLight(world);
		CreateDirectionalLight(world, { 0.0f, -1.0f, 0.0f });

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

	static aoe::Entity CreateAstroObject(
		aoe::World& world,
		aoe::Relationeer<aoe::TransformComponent>& relationeer,
		aoe::DX11ModelManager& model_manager,
		aoe::DX11TextureManager& texture_manager,
		const std::wstring& texture_path)
	{
		using namespace aoe;

		ModelId model_id = model_manager.Load(L"/Content/Sphere.fbx", ModelLoaderOptions::kFlipUVs);
		TextureId texture_id = texture_manager.LoadRGBA(texture_path);

		Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.8f, 0.8f, 0.8f };
		material.shininess = 32.0f;

		Entity astro_object = world.CreateEntity();
		world.AddComponent<TransformComponent>(astro_object);
		world.AddComponent<RenderComponent>(astro_object, model_id, texture_id, material);

		Transform axis_transform{};
		Entity axis = DebugUtils::CreateAxis(world, relationeer, axis_transform);

		relationeer.SetParent(axis, astro_object);
		return astro_object;
	}

	static aoe::Entity CreateAmbientLight(aoe::World& world) {
		using namespace aoe;

		Entity ambient_light = world.CreateEntity();
		world.AddComponent<AmbientLightComponent>(ambient_light, Vector3f(1.0f, 1.0f, 1.0f), 0.25f);

		return ambient_light;
	}

	static aoe::Entity CreateDirectionalLight(aoe::World& world, aoe::Vector3f direction) {
		using namespace aoe;

		Entity directional_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(directional_light);
		world.AddComponent<DirectionalLightComponent>(directional_light, Vector3f(1.0f, 1.0f, 1.0f));

		auto transform_component = world.GetComponent<TransformComponent>(directional_light);

		Transform& transform = transform_component->transform;
		transform.rotation = Quaternion::RotateFromTo(transform.GetForward(), direction.Normalized());

		return directional_light;
	}

	static aoe::Entity CreatePointLight(
		aoe::World& world,
		aoe::Relationeer<aoe::TransformComponent>& relationeer)
	{
		using namespace aoe;

		Entity point_light = world.CreateEntity();
		world.AddComponent<TransformComponent>(point_light);
		world.AddComponent<PointLightComponent>(point_light, Vector3f(1.0f, 1.0f, 1.0f));

		Entity sphere = DebugUtils::CreateSphere(world, {}, Colors::kWhite);
		relationeer.SetParent(sphere, point_light);

		return point_light;
	}
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	SolarSystemScene scene(application);
	application.Start(scene);

	return 0;
}