#pragma once

#include "../ECS/World.h"

#include "TransformComponent.h"
#include "Relationeer.h"

namespace aoe {

class TransformUtils {
public:
	TransformUtils() = delete;

	static Vector3f GetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform_component->position;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return world_matrix * transform_component->position;
	}

	static void SetGlobalPosition(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		Vector3f position)
	{
		auto transform_component = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform_component->position = position;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		transform_component->position = world_matrix.Inverse() * position;
	}

	static Quaternion GetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform_component->rotation;
		}

		Entity parent = relationeer.GetParent(entity);
		return GetGlobalRotation(world, relationeer, parent) * transform_component->rotation;
	}

	static void SetGlobalRotation(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity,
		Quaternion rotation)
	{
		auto transform_component = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform_component->rotation = rotation;
		}

		Entity parent = relationeer.GetParent(entity);
		Quaternion parent_rotation = GetGlobalRotation(world, relationeer, parent);
		transform_component->rotation = parent_rotation.Inverse() * rotation;
	}

	static Matrix4f GetGlobalWorldMatrix(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity)
	{
		auto transform_component = world.Get<TransformComponent>(entity);
		Matrix4f world_matrix = transform_component->GetWorldMatrix();

		if (IsRoot(world, relationeer, entity)) {
			return world_matrix;
		}

		Entity parent = relationeer.GetParent(entity);
		Matrix4f parent_world_matrix = GetGlobalWorldMatrix(world, relationeer, parent);
		return parent_world_matrix * world_matrix;
	}

	static Vector3f GetGlobalRight(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kRight;
	}

	static Vector3f GetGlobalLeft(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kLeft;
	}

	static Vector3f GetGlobalUp(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kUp;
	}

	static Vector3f GetGlobalDown(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kDown;
	}

	static Vector3f GetGlobalForward(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
		return world_matrix * Math::kForward;
	}

	static Vector3f GetGlobalBackward(
		World& world,
		Relationeer<TransformComponent>& relationeer,
		Entity entity) 
	{
		Matrix4f world_matrix = GetGlobalWorldMatrix(world, relationeer, entity);
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