#include <functional>

#include "../Application/Application.h"
#include "../Core/Math.h"
#include "../ECS/World.h"
#include "../Resources/ModelLoader.h"
#include "../Game/TransformComponent.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformUtils.h"
#include "../Game/IECSSystem.h"
#include "../Game/SystemsPool.h"
#include "../Renderer/DX11RenderSystem.h"
#include "../Renderer/AmbientLightComponent.h"
#include "../Renderer/DirectionalLightComponent.h"
#include "../Renderer/LineComponent.h"
#include "../Renderer/CameraComponent.h"
#include "../Renderer/Colors.h"
#include "../Renderer/DebugUtils.h"
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
		aoe::Entity entity0 = world_.Create();
		world_.Add<aoe::TransformComponent>(entity0);
		auto transform_component0 = world_.Get<aoe::TransformComponent>(entity0);
		transform_component0->transform.position = { 0.0f, 0.0f, 4.0f };
		transform_component0->transform.scale = { 1.0f, 2.0f, 0.5f };

		aoe::ModelId model_id = model_manager_.Load(L"/Content/Dice_d4.fbx", aoe::ModelLoaderOptions::kFlipUVs);
		aoe::TextureId texture_id = texture_manager_.LoadRGBA(L"/Content/Dice_d4_Albedo.png");

		aoe::Material material;
		material.diffuse = { 1.0f, 1.0f, 1.0f };
		material.specular = { 0.1f, 0.1f, 0.1f };
		material.shininess = 32.0f;
		world_.Add<aoe::RenderComponent>(entity0, model_id, texture_id, material);

		//world_.Add<aoe::LineComponent>(entity0);
		//auto line_component_x = world_.Get<aoe::LineComponent>(entity0);
		//line_component_x->color = aoe::Colors::kRed;
		//line_component_x->points = { 
		//	{0.0f, 0.0f, 0.0f}, 
		//	{1.0f, 0.0f, 0.0f}, 
		//	{1.0f, 1.0f, 0.0f}, 
		//	{0.0f, 0.0f, 0.0f} 
		//};
		
		aoe::Entity entity1 = world_.Create();
		world_.Add<aoe::TransformComponent>(entity1);
		auto transform_component1 = world_.Get<aoe::TransformComponent>(entity1);
		transform_component1->transform.position = { 1.3f, 0.0f, 5.0f };

		auto other_material = material;
		other_material.diffuse = { 0.3, 1, 0.3 };
		world_.Add<aoe::RenderComponent>(entity1, model_id, texture_id, other_material);

		aoe::Transform axis_transform{};
		axis_transform.scale = { 3.0f, 3.0f, 3.0f };
		auto axis = aoe::DebugUtils::CreateAxis(world_, relationeer_, axis_transform);
		relationeer_.SetParent(axis, entity1);

		auto sphere = aoe::DebugUtils::CreateSphere(world_);
		relationeer_.SetParent(sphere, entity0);

		aoe::Transform transform;
		transform.position = { 1.0f, 0.0f, 0.0f };
		transform.rotation = aoe::Quaternion::FromEulerAngles(0.0f, aoe::Math::kPiDiv4, 0.0f);
		aoe::DebugUtils::CreateCube(world_, transform);

		aoe::DebugUtils::CreateGrid(world_, { 20, 0, 20 }, {}, aoe::Colors::kWhite);
		aoe::DebugUtils::CreateSphere(world_);

		aoe::Entity ambient_light = world_.Create();
		world_.Add<aoe::AmbientLightComponent>(ambient_light);
		auto ambient_light_component = world_.Get<aoe::AmbientLightComponent>(ambient_light);
		ambient_light_component->color = { 1.0f, 1.0f, 1.0f };
		ambient_light_component->intensity = 0.25f;

		aoe::Entity direction_light = world_.Create();
		world_.Add<aoe::TransformComponent>(direction_light);
		world_.Add<aoe::DirectionalLightComponent>(direction_light);
		auto direction_transform_component = world_.Get<aoe::TransformComponent>(direction_light);
		auto direction_light_component = world_.Get<aoe::DirectionalLightComponent>(direction_light);
		direction_light_component->color = { 1.0f, 1.0f, 1.0f };

		aoe::Entity camera = world_.Create();
		world_.Add<aoe::TransformComponent>(camera);
		world_.Add<aoe::CameraComponent>(camera);
		world_.Add<aoe::FlyCameraComponent>(camera);
		auto camera_transform_component = world_.Get<aoe::TransformComponent>(camera);
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

		systems_pool_.PushSystem<aoe::FlyCameraSystem>(service_provider_);
		systems_pool_.PushSystem<aoe::DX11RenderSystem>(service_provider_);
		systems_pool_.Initialize();
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