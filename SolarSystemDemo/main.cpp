#include "../Core/Math.h"
#include "../Application/Application.h"
#include "../ECS/World.h"
#include <functional>

class TransformComponent {
public:
	aoe::Vector3 position_;
	aoe::Vector3 scale_;
	aoe::Quaternion rotation_;
};

template<typename TComponent>
class Relationeer {
public:
	class Relations {
		friend class Relationeer<TComponent>;

	public:
		Relations()
			: parent(aoe::Entity::Null())
			, children()
		{}

	private:
		aoe::Entity parent;
		std::vector<aoe::Entity> children;
	};

	Relationeer(aoe::World& world)
		: world_(world)
	{}

	bool HasRelations(aoe::Entity entity) const {
		return world_.Has<Relations>(entity);
	}

	void BreakRelations(aoe::Entity entity) {
		world_.Remove<Relations>(entity);
	}

	aoe::Entity GetParent(aoe::Entity child) const {
		aoe::ComponentHandler<Relations> relations = world_.Get<Relations>(child);
		return relations->parent;
	}

	void SetParent(aoe::Entity child, aoe::Entity parent) {
		AssertHasComponent(child);
		AssertHasComponent(parent);
		
		aoe::ComponentHandler<Relations> child_relations = GetRelations(child);
		aoe::ComponentHandler<Relations> parent_relations = GetRelations(parent);

		AOE_ASSERT_MSG(child.GetId() != parent.GetId(), "Entity can't be parent for itself.");
		AOE_ASSERT_MSG(!IsChildrenOf(parent, child), "Parent doesn't have to be a child.");
		
		MakeRoot(child);
		child_relations->parent = parent;
		parent_relations->children.push_back(child);

		// TODO: subscription to add/remove of component
		// TODO: subscribe to entity create/delete
	}

	void MakeRoot(aoe::Entity child) {
		aoe::ComponentHandler<Relations> child_relations = world_.Get<Relations>(child);
		aoe::Entity parent = child_relations->parent;

		if (parent.IsNull()) {
			return;
		}

		aoe::ComponentHandler<Relations> parent_relations = world_.Get<Relations>(parent);
		std::vector<aoe::Entity>& children = parent_relations->children;

		auto it = std::find(children.begin(), children.end(), child);
		children.erase(it);
	}

	const std::vector<aoe::Entity>& GetChildren(aoe::Entity entity) const {
		aoe::ComponentHandler<Relations> relations = world_.Get<Relations>(entity);
		return relations->children;
	}

	bool IsChildrenOf(aoe::Entity child, aoe::Entity parent) const {
		aoe::ComponentHandler<Relations> temp = world_.Get<Relations>(child);

		do {
			if (temp->parent.IsNull()) {
				return false;
			}

			if (temp->parent == parent) {
				return true;
			}

			temp = world_.Get<Relations>(temp->parent);
		} while (true);
	}

private:
	aoe::World& world_;
	
	void AssertHasComponent(aoe::Entity entity) const {
		AOE_ASSERT_MSG(world_.Has<TComponent>(entity), "Entity doesn't contain relaited component.");
	}

	void AssertHasRelations(aoe::Entity entity) const {
		AOE_ASSERT_MSG(HasRelations(entity), "Entity hasn't relations.");
	}

	aoe::ComponentHandler<Relations> GetRelations(aoe::Entity entity) const {
		if (!HasRelations(entity)) {
			world_.Add<Relations>(entity);
		}

		return world_.Get<Relations>(entity);
	}
};

class TransformConvertor {
public:
	TransformConvertor(Relationeer<TransformComponent>& relationeer)
		: relationeer_(relationeer)
	{}

	aoe::Vector3 GetGlobalPosition() const {
		return {};
	}

	aoe::Quaternion GetGlobalRotation() const {
		return {};
	}

private:
	Relationeer<TransformComponent>& relationeer_;
};

class SolarSystemScene : public aoe::IScene {
public:
	SolarSystemScene(const aoe::Window& window)
		: window_(window)
	{}

	void Initialize() override {};

	void Terminate() override {};

	void PerTickUpdate(float dt) override {}

	void PerFrameUpdate(float dt) override {}

private:
	const aoe::Window& window_;
};

int main() {
	aoe::World world;
	Relationeer<TransformComponent> relationeer(world);
	
	aoe::Entity parent = world.Create();
	aoe::Entity child = world.Create();

	world.Add<TransformComponent>(parent);
	world.Add<TransformComponent>(child);

	relationeer.SetParent(child, parent);
	bool test0 = relationeer.IsChildrenOf(child, parent);
	bool test1 = relationeer.IsChildrenOf(parent, child);

	auto lmbd0 = []() {
		AOE_LOG_INFO("Some info");
	};

	auto lmbd1 = []() {
		AOE_LOG_INFO("Other info");
	};

	auto lmbd2 = lmbd0;

	bool test2 = lmbd0 == lmbd1;
	bool test3 = lmbd0 == lmbd2;

	//aoe::Application application(L"Game", 800, 600);

	//SolarSystemScene scene(application.GetWindow());
	//application.Start(scene);

	return 0;
}