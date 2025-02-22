#pragma once

#include "ECSSystemBase.h"
#include "Relationeer.h"
#include "TransformComponent.h"
#include "TransformChangedComponent.h"

namespace aoe {

class TransformSystem : public ECSSystemBase {
public:
	void Initialize(const aoe::ServiceProvider& service_provider) {
		ECSSystemBase::Initialize(service_provider);

		relationeer_ = service_provider.TryGetService<Relationeer<TransformComponent>>();
		AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");
	}

	void Update(float dt) override {
		RemoveNotificationComponents();
		UpdateTransformComponents();
	}

private:
	Relationeer<TransformComponent>* relationeer_;

	void RemoveNotificationComponents() {
		for (Entity entity : FilterEntities<TransformChangedComponent>()) {
			RemoveComponent<TransformChangedComponent>(entity);
		}
	}

	void UpdateTransformComponents() {
		const Matrix4f origin = Matrix4f::Identity();

		for (Entity entity : FilterEntities<TransformComponent>()) {
			if (relationeer_->IsRoot(entity)) {
				UpdateTransformComponents(entity, origin, false);
			}
		}
	}

	void UpdateTransformComponents(
		Entity entity, 
		const Matrix4f& transformation,
		bool has_changed) 
	{
		auto transform_component = GetComponent<TransformComponent>(entity);
		has_changed |= transform_component->HasChanged();
		
		if (has_changed) {
			UpdateTransformComponent(entity, transform_component, transformation);
		}

		const Matrix4f& global_world_matrix = transform_component->global_world_matrix_;

		for (Entity children : relationeer_->GetChildren(entity)) {
			UpdateTransformComponents(children, global_world_matrix, has_changed);
		}
	}

	void UpdateTransformComponent(
		Entity entity,
		CH<TransformComponent> transform_component, 
		const Matrix4f& transformation)
	{
		const Matrix4f world_matrix = transform_component->GetWorldMatrix();
		const Matrix4f global_world_matrix = transformation * world_matrix;

		transform_component->SetGlobalWorldMatrix(global_world_matrix);
		AddComponent<TransformChangedComponent>(entity);
	}
};

} // namespace aoe