#pragma once

template<typename TComponent>
class Relationeer {
public:
	class RelationsComponent {
	private:
		friend class Relationeer<TComponent>;

	public:
		RelationsComponent()
			: parent(aoe::Entity::Null())
			, children()
		{}

	private:
		aoe::Entity parent;
		std::vector<aoe::Entity> children;
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

	bool HasRelations(aoe::Entity entity) const {
		return world_.Has<RelationsComponent>(entity);
	}

	void BreakRelations(aoe::Entity entity) {
		world_.Remove<RelationsComponent>(entity);
	}

	aoe::Entity GetParent(aoe::Entity child) const {
		auto relations = world_.Get<RelationsComponent>(child);
		return relations->parent;
	}

	void SetParent(aoe::Entity child, aoe::Entity parent) {
		AssertHasComponent(child);
		AssertHasComponent(parent);

		aoe::ComponentHandler<RelationsComponent> child_relations = GetOrCreateRelations(child);
		aoe::ComponentHandler<RelationsComponent> parent_relations = GetOrCreateRelations(parent);

		AOE_ASSERT_MSG(child.GetId() != parent.GetId(), "Entity can't be parent for itself.");
		AOE_ASSERT_MSG(!IsChildrenOf(parent, child), "Parent doesn't have to be a child.");

		MakeRoot(child);
		child_relations->parent = parent;
		parent_relations->children.push_back(child);
	}

	void MakeRoot(aoe::Entity child) {
		auto child_relations = world_.Get<RelationsComponent>(child);
		aoe::Entity parent = child_relations->parent;

		if (parent.IsNull()) {
			return;
		}

		auto parent_relations = world_.Get<RelationsComponent>(parent);
		std::vector<aoe::Entity>& children = parent_relations->children;

		auto it = std::find(children.begin(), children.end(), child);
		children.erase(it);
	}

	const std::vector<aoe::Entity>& GetChildren(aoe::Entity entity) const {
		auto relations = world_.Get<RelationsComponent>(entity);
		return relations->children;
	}

	bool IsChildrenOf(aoe::Entity child, aoe::Entity parent) const {
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
	aoe::World& world_;

	void AssertHasComponent(aoe::Entity entity) const {
		AOE_ASSERT_MSG(world_.Has<TComponent>(entity), "Entity doesn't contain relaited component.");
	}

	void AssertHasRelations(aoe::Entity entity) const {
		AOE_ASSERT_MSG(HasRelations(entity), "Entity hasn't relations.");
	}

	aoe::ComponentHandler<RelationsComponent> GetOrCreateRelations(aoe::Entity entity) const {
		if (!HasRelations(entity)) {
			world_.Add<RelationsComponent>(entity);
		}

		return world_.Get<RelationsComponent>(entity);
	}

	void OnRelatedComponentRemoved(aoe::Entity entity) {
		BreakRelations(entity);
	}

	void OnRelationsComponentRemoved(aoe::Entity entity) {
		BreakHierarchy(entity);
	}

	void BreakHierarchy(aoe::Entity entity) {
		auto relations = world_.Get<RelationsComponent>(entity);

		for (aoe::Entity child : relations->children) {
			auto child_relations = world_.Get<RelationsComponent>(child);
			child_relations->parent = aoe::Entity::Null();
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