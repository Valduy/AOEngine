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
		return GetGlobalRotation(world, relationeer, parent) * transform->rotation;
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
		Quaternion parent_rotation = GetGlobalRotation(world, relationeer, parent);
		transform->rotation = parent_rotation.Inverse() * rotation;
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

	static Vector3 GetGlobalRight(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kRight;
	}

	static Vector3 GetGlobalLeft(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kLeft;
	}

	static Vector3 GetGlobalUp(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kUp;
	}

	static Vector3 GetGlobalDown(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kDown;
	}

	static Vector3 GetGlobalForward(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kForward;
	}

	static Vector3 GetGlobalBackward(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4 world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kBackward;
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