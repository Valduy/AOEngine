#pragma once

#include "../ECS/World.h"

namespace aoe {

template<typename TComponent>
class Relationeer {
public:
	Relationeer(aoe::World& world)
		: world_(world)
		, relations_()
	{
		world_.ComponentAdded<TComponent>().Attach(
			*this, &Relationeer<TComponent>::OnComponentAdded);
		world_.ComponentRemoved<TComponent>().Attach(
			*this, &Relationeer<TComponent>::OnComponentRemoved);
	}

	~Relationeer() {
		world_.ComponentAdded<TComponent>().Detach(
			*this, &Relationeer<TComponent>::OnComponentAdded);
		world_.ComponentRemoved<TComponent>().Detach(
			*this, &Relationeer<TComponent>::OnComponentRemoved);
	}

	bool HasRelations(Entity entity) const {
		return relations_.Has(entity.GetId());
	}

	Entity GetParent(Entity child) const {
		AssertHasRelations(child);
		const Relations& relations = GetRelations(child);
		return relations.parent;
	}

	void SetParent(Entity child, Entity parent) {
		AssertHasRelations(child);
		AssertHasRelations(parent);

		Relations& child_relations = GetRelations(child);
		Relations& parent_relations = GetRelations(parent);

		AOE_ASSERT_MSG(child.GetId() != parent.GetId(), "Entity can't be parent for itself.");
		AOE_ASSERT_MSG(!IsChildOf(parent, child), "Parent doesn't have to be a child.");

		MakeRoot(child);
		child_relations.parent = parent;
		parent_relations.children.push_back(child);
	}

	void MakeRoot(Entity child) {
		Relations& child_relations = GetRelations(child);
		Entity parent = child_relations.parent;

		if (parent.IsNull()) {
			return;
		}

		child_relations.parent = Entity::Null();

		Relations& parent_relations = GetRelations(parent);
		std::vector<Entity>& parent_children = parent_relations.children;

		auto it = std::remove(parent_children.begin(), parent_children.end(), child);
		parent_children.erase(it, parent_children.end());
	}

	const std::vector<Entity>& GetChildren(Entity entity) const {
		const Relations& relations = GetRelations(entity);
		return relations.children;
	}

	bool IsRoot(Entity entity) const {
		return GetParent(entity).IsNull();
	}

	bool IsChildOf(Entity child, Entity parent) const {
		const Relations* temp = &GetRelations(child);

		do {
			if (temp->parent.IsNull()) {
				return false;
			}

			if (temp->parent == parent) {
				return true;
			}

			temp = &GetRelations(temp->parent);
		} while (true);
	}

private:
	struct Relations {
		Entity parent;
		std::vector<Entity> children;

		Relations()
			: parent(Entity::Null())
			, children()
		{}
	};

	World& world_;
	SparseMap<Relations> relations_;

	void AssertHasRelations(Entity entity) const {
		AOE_ASSERT_MSG(HasRelations(entity), "Entity hasn't relations.");
	}

	Relations& GetRelations(Entity entity) {
		return relations_.Get(entity.GetId());
	}

	const Relations& GetRelations(Entity entity) const {
		return relations_.Get(entity.GetId());
	}

	void AddRelations(Entity entity) {
		relations_.Emplace(entity.GetId());
	}

	void RemoveRelations(Entity entity) {
		relations_.Remove(entity.GetId());
	}

	void OnComponentAdded(Entity entity) {
		AddRelations(entity);
	}

	void OnComponentRemoved(Entity entity) {
		BreakHierarchy(entity);
		RemoveRelations(entity);
	}

	void BreakHierarchy(Entity entity) {
		MakeRoot(entity);
		Relations& relations = GetRelations(entity);

		for (Entity child : relations.children) {
			Relations& child_relations = GetRelations(child);
			child_relations.parent = Entity::Null();
		}

		relations.children.clear();
	}
};

} // namespace aoe