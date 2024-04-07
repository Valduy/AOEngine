#pragma once

#include "../ECS/World.h"

namespace aoe {

template<typename TComponent>
class Relationeer {
public:
	class RelationsComponent {
	private:
		friend class Relationeer<TComponent>;

	public:
		RelationsComponent()
			: parent(Entity::Null())
			, children()
		{}

	private:
		Entity parent;
		std::vector<Entity> children;
	};

	Relationeer(aoe::World& world)
		: world_(world)
	{
		world_.ComponentRemoved<TComponent>().Attach(
			*this, &Relationeer<TComponent>::OnRelatedComponentRemoved);
		world_.ComponentRemoved<RelationsComponent>().Attach(
			*this, &Relationeer<TComponent>::OnRelationsComponentRemoved);
	}

	~Relationeer() {
		world_.ComponentRemoved<TComponent>().Detach(
			*this, &Relationeer<TComponent>::OnRelatedComponentRemoved);
		world_.ComponentRemoved<RelationsComponent>().Detach(
			*this, &Relationeer<TComponent>::OnRelationsComponentRemoved);
	}

	bool HasRelations(Entity entity) const {
		return world_.Has<RelationsComponent>(entity);
	}

	void BreakRelations(Entity entity) {
		world_.Remove<RelationsComponent>(entity);
	}

	Entity GetParent(Entity child) const {
		auto relations = world_.Get<RelationsComponent>(child);
		return relations->parent;
	}

	void SetParent(Entity child, Entity parent) {
		AssertHasComponent(child);
		AssertHasComponent(parent);

		auto child_relations = GetOrCreateRelations(child);
		auto parent_relations = GetOrCreateRelations(parent);

		AOE_ASSERT_MSG(child.GetId() != parent.GetId(), "Entity can't be parent for itself.");
		AOE_ASSERT_MSG(!IsChildrenOf(parent, child), "Parent doesn't have to be a child.");

		MakeRoot(child);
		child_relations->parent = parent;
		parent_relations->children.push_back(child);
	}

	void MakeRoot(Entity child) {
		auto child_relations = world_.Get<RelationsComponent>(child);
		Entity parent = child_relations->parent;

		if (parent.IsNull()) {
			return;
		}

		auto parent_relations = world_.Get<RelationsComponent>(parent);
		std::vector<Entity>& children = parent_relations->children;

		auto it = std::find(children.begin(), children.end(), child);
		children.erase(it);
	}

	const std::vector<Entity>& GetChildren(Entity entity) const {
		auto relations = world_.Get<RelationsComponent>(entity);
		return relations->children;
	}

	bool IsChildrenOf(Entity child, Entity parent) const {
		auto temp = world_.Get<RelationsComponent>(child);

		do {
			if (temp->parent.IsNull()) {
				return false;
			}

			if (temp->parent == parent) {
				return true;
			}

			temp = world_.Get<RelationsComponent>(temp->parent);
		} while (true);
	}

private:
	World& world_;

	void AssertHasComponent(Entity entity) const {
		AOE_ASSERT_MSG(world_.Has<TComponent>(entity), "Entity doesn't contain relaited component.");
	}

	void AssertHasRelations(Entity entity) const {
		AOE_ASSERT_MSG(HasRelations(entity), "Entity hasn't relations.");
	}

	auto GetOrCreateRelations(Entity entity) const {
		if (!HasRelations(entity)) {
			world_.Add<RelationsComponent>(entity);
		}

		return world_.Get<RelationsComponent>(entity);
	}

	void OnRelatedComponentRemoved(Entity entity) {
		BreakRelations(entity);
	}

	void OnRelationsComponentRemoved(Entity entity) {
		BreakHierarchy(entity);
	}

	void BreakHierarchy(Entity entity) {
		auto relations = world_.Get<RelationsComponent>(entity);

		for (Entity child : relations->children) {
			auto child_relations = world_.Get<RelationsComponent>(child);
			child_relations->parent = Entity::Null();
		}

		if (relations->parent.IsNull()) {
			return;
		}

		auto parent_relations = world_.Get<RelationsComponent>(relations->parent);
		parent_relations->children.erase(std::find(
			parent_relations->children.begin(),
			parent_relations->children.end(),
			entity));
	}
};

} // namespace aoe