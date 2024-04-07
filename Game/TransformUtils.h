#pragma once

#include "../ECS/World.h"

#include "TransformComponent.h"
#include "Relationeer.h"

namespace aoe {

class TransformUtils {
public:
	TransformUtils() = delete;

	static Vector3 GetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform->position;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return world_matrix * transform->position;
	}

	static void SetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		Vector3 position)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform->position = position;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		transform->position = world_matrix.Inverse() * position;
	}

	static Quaternion GetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform->rotation;
		}

		Entity parent = relationeer.GetParent(entity);
		auto parent_transform = world.Get<TransformComponent>(parent);
		return transform->rotation * parent_transform->rotation;
	}

	static void SetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		Quaternion rotation)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform->rotation = rotation;
		}

		Entity parent = relationeer.GetParent(entity);
		auto parent_transform = world.Get<TransformComponent>(parent);
		transform->rotation = parent_transform->rotation.Inverse() * rotation;
	}

	static Matrix4 GetGlobalWorldMatrix(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);
		Matrix4 model_matrix = transform->GetWorldMatrix();

		if (IsRoot(world, relationeer, entity)) {
			return model_matrix;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return world_matrix * model_matrix;
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