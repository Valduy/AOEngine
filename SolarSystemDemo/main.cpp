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
#include "../Renderer/DX11RenderDataUpdateSystem.h"
#include "../Renderer/DX11RenderSystem.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

class SolarSystemScene : public aoe::IScene {
public:
	SolarSystemScene(const aoe::Window& window)
		: window_(window)
		, world_()
		, relationeer_(world_)
		, service_provider_()
	{}

	void Initialize() override {
		// experiments begin
		world_.EntityCreated.Attach(*this, &SolarSystemScene::TestEntityCreated);
		world_.EntityDestroyed.Attach(*this, &SolarSystemScene::TestEntityDestroyed);

		aoe::Entity parent = world_.Create();
		aoe::Entity child = world_.Create();

		world_.Add<aoe::TransformComponent>(parent);
		world_.Add<aoe::TransformComponent>(child);

		auto parent_transform = world_.Get<aoe::TransformComponent>(parent);
		auto child_transform = world_.Get<aoe::TransformComponent>(child);

		parent_transform->position = { 0.0f, 0.0f, 1.0f };
		parent_transform->rotation = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 4 });

		child_transform->position = { 0.0f, 0.0f, 1.0f };
		child_transform->rotation = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 4 });

		relationeer_.SetParent(child, parent);

		bool isChildChild = relationeer_.IsChildrenOf(child, parent);
		bool isParentChild = relationeer_.IsChildrenOf(parent, child);

		auto test0 = aoe::TransformUtils::GetGlobalPosition(world_, relationeer_, parent);
		auto test1 = aoe::TransformUtils::GetGlobalPosition(world_, relationeer_, child);

		auto test3 = aoe::TransformUtils::GetGlobalRotation(world_, relationeer_, parent).ToEulerAngles();
		auto test4 = aoe::TransformUtils::GetGlobalRotation(world_, relationeer_, child).ToEulerAngles();

		aoe::TransformUtils::SetGlobalPosition(world_, relationeer_, child, { 0.0f, 0.0f, 2.0f });
		auto test5 = child_transform->position;

		auto quat = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 2 });
		aoe::TransformUtils::SetGlobalRotation(world_, relationeer_, child, quat);
		auto test6 = child_transform->rotation.ToEulerAngles();

		world_.Destroy(parent);
		world_.Destroy(child);
		world_.Validate();
		//experiments end
		
		aoe::Entity entity = world_.Create();
		world_.Add<aoe::TransformComponent>(entity);

		aoe::Material material;
		material.ambient = { 0.1f, 0.1f, 0.1f };
		material.diffuse = { 1.0f, 0.3f, 0.3f };
		material.specular = { 0.1f, 0.1f, 0.1f };
		material.shininess = 32.0f;
		world_.Add<aoe::RenderComponent>(entity, model_manager_.GetDefault(), texture_manager_.GetDefault(), material);

		service_provider_.AddService(&world_);
		service_provider_.AddService(&relationeer_);

		systems_pool_.PushSystem<aoe::DX11RenderDataUpdateSystem>(window_, service_provider_);
		systems_pool_.PushSystem<aoe::DX11RenderSystem>(window_, service_provider_);
		systems_pool_.Initialize();
	};

	void Terminate() override {
		systems_pool_.Terminate();
	};

	void PerTickUpdate(float dt) override {
		systems_pool_.PerTickUpdate(dt);
	}

	void PerFrameUpdate(float dt) override {
		systems_pool_.PerFrameUpdate(dt);
	}

private:
	const aoe::Window& window_;

	aoe::World world_;
	aoe::Relationeer<aoe::TransformComponent> relationeer_;

	aoe::DX11ModelManager model_manager_;
	aoe::DX11TextureManager texture_manager_;

	aoe::ServiceProvider service_provider_;
	aoe::SystemsPool systems_pool_;

	void TestEntityCreated(aoe::Entity entity) {
		int test = 0;
	}

	void TestEntityDestroyed(aoe::Entity entity) {
		int test = 0;
	}
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	SolarSystemScene scene(application.GetWindow());
	application.Start(scene);

	return 0;
}