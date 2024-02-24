#include <functional>

#include "../Application/Application.h"

#include "../Core/Math.h"
#include "../ECS/World.h"

#include "TransformComponent.h"
#include "Relationeer.h"
#include "TransformUtils.h"

class SolarSystemScene : public aoe::IScene {
public:
	SolarSystemScene(const aoe::Window& window)
		: window_(window)
		, world_()
		, relationeer_(world_)
	{}

	void Initialize() override {
		world_.EntityCreated.Attach(*this, &SolarSystemScene::TestEntityCreated);
		world_.EntityDestroyed.Attach(*this, &SolarSystemScene::TestEntityDestroyed);

		aoe::Entity parent = world_.Create();
		aoe::Entity child = world_.Create();

		world_.Add<TransformComponent>(parent);
		world_.Add<TransformComponent>(child);

		auto parent_transform = world_.Get<TransformComponent>(parent);
		auto child_transform = world_.Get<TransformComponent>(child);

		parent_transform->position = { 0.0f, 0.0f, 1.0f };
		parent_transform->rotation = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 4 });

		child_transform->position = { 0.0f, 0.0f, 1.0f };
		child_transform->rotation = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 4 });

		relationeer_.SetParent(child, parent);

		bool isChildChild = relationeer_.IsChildrenOf(child, parent);
		bool isParentChild = relationeer_.IsChildrenOf(parent, child);

		auto test0 = TransformUtils::GetGlobalPosition(world_, relationeer_, parent);
		auto test1 = TransformUtils::GetGlobalPosition(world_, relationeer_, child);

		auto test3 = TransformUtils::GetGlobalRotation(world_, relationeer_, parent).ToEulerAngles();
		auto test4 = TransformUtils::GetGlobalRotation(world_, relationeer_, child).ToEulerAngles();

		TransformUtils::SetGlobalPosition(world_, relationeer_, child, { 0.0f, 0.0f, 2.0f });
		auto test5 = child_transform->position;

		auto quat = aoe::Quaternion::FromEulerAngles({ 0.0f, 0.0f, aoe::Math::kPi / 2 });
		TransformUtils::SetGlobalRotation(world_, relationeer_, child, quat);
		auto test6 = child_transform->rotation.ToEulerAngles();

		world_.Destroy(parent);
		world_.Destroy(child);

		world_.Validate();
	};

	void Terminate() override {};

	void PerTickUpdate(float dt) override {}

	void PerFrameUpdate(float dt) override {}

private:
	const aoe::Window& window_;

	aoe::World world_;
	Relationeer<TransformComponent> relationeer_;

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