#pragma once

#include "../ECS/World.h"

#include "TransformComponent.h"
#include "Relationeer.h"

class TransformUtils {
public:
	TransformUtils() = delete;

	static aoe::Vector3 GetGlobalPosition(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform->position;
		}

		aoe::Entity parent = relationeer.GetParent(entity);
		aoe::Matrix4 world_matrix = GetGlobalModelMatrix(world, relationeer, parent);
		return world_matrix * transform->position;
	}

	static void SetGlobalPosition(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity,
		aoe::Vector3 position)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform->position = position;
		}

		aoe::Entity parent = relationeer.GetParent(entity);
		aoe::Matrix4 world_matrix = GetGlobalModelMatrix(world, relationeer, parent);
		transform->position = world_matrix.Inverse() * position;
	}

	static aoe::Quaternion GetGlobalRotation(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			return transform->rotation;
		}

		aoe::Entity parent = relationeer.GetParent(entity);
		auto parent_transform = world.Get<TransformComponent>(parent);
		return transform->rotation * parent_transform->rotation;
	}

	static void SetGlobalRotation(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity,
		aoe::Quaternion rotation)
	{
		auto transform = world.Get<TransformComponent>(entity);

		if (IsRoot(world, relationeer, entity)) {
			transform->rotation = rotation;
		}

		aoe::Entity parent = relationeer.GetParent(entity);
		auto parent_transform = world.Get<TransformComponent>(parent);
		transform->rotation = parent_transform->rotation.Inverse() * rotation;
	}

	static aoe::Matrix4 GetGlobalModelMatrix(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity)
	{
		auto transform = world.Get<TransformComponent>(entity);
		aoe::Matrix4 model_matrix = transform->GetModelMatrix();

		if (IsRoot(world, relationeer, entity)) {
			return model_matrix;
		}

		aoe::Entity parent = relationeer.GetParent(entity);
		aoe::Matrix4 world_matrix = GetGlobalModelMatrix(world, relationeer, parent);
		return world_matrix * model_matrix;
	}

private:
	static bool IsRoot(
		aoe::World& world,
		Relationeer<TransformComponent>& relationeer,
		aoe::Entity entity)
	{
		return !relationeer.HasRelations(entity) || relationeer.GetParent(entity).IsNull();
	}

};