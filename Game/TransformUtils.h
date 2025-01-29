#pragma once

#include "../ECS/World.h"

#include "TransformComponent.h"
#include "Relationeer.h"

namespace aoe {

class TransformUtils {
public:
	TransformUtils() = delete;

	static Transform GetGlobalTransform(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform_component->GetTransform();
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f local_world_matrix = transform_component->GetWorldMatrix();
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		Matrix4f global_world_matrix = parent_world_matrix * local_world_matrix;

		return global_world_matrix;
	}

	static void SetGlobalTransform(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		const Transform& transform)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform_component->SetTransform(transform);
			return;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f global_world_matrix = transform.ToMatrix();
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		Matrix4f local_world_matrix = parent_world_matrix.Inverse() * global_world_matrix;

		transform_component->SetWorldMatrix(local_world_matrix);
	}

	static Vector3f GetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform_component->GetPosition();
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return parent_world_matrix * transform_component->GetPosition();
	}

	static void SetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		const Vector3f& position)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform_component->SetPosition(position);
			return;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		transform_component->SetPosition(parent_world_matrix.Inverse() * position);
	}

	static Quaternion GetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform_component->GetRotation();
		}

		Entity parent = relationeer.GetParent(entity);
		Quaternion parent_rotation = GetGlobalRotation(world, relationeer, parent);
		return parent_rotation * transform_component->GetRotation();
	}

	static void SetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		const Quaternion& rotation)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform_component->SetRotation(rotation);
			return;
		}

		Entity parent = relationeer.GetParent(entity);
		Quaternion parent_rotation = GetGlobalRotation(world, relationeer, parent);
		transform_component->SetRotation(parent_rotation.Inverse() * rotation);
	}

	static Matrix4f GetGlobalWorldMatrix(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.GetComponent<TransformComponent>(entity);
		const Matrix4f& world_matrix = transform_component->GetWorldMatrix();

		if (IsRoot(world, relationeer, entity)) {
			return world_matrix;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return parent_world_matrix * world_matrix;
	}

private:
	static bool IsRoot(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		return !relationeer.HasRelations(entity) || relationeer.GetParent(entity).IsNull();
	}

};

} // namespace aoe