#include "pch.h"

#include <unordered_set>

#include "../ECS/World.h"

namespace aoe_tests{
namespace ecs_tests {

struct TestComponentA {};

struct TestComponentB {};

struct TestComponentC {};

struct EntityHashFuction {
	size_t operator()(const aoe::Entity& entity) const {
		aoe::Version version = entity.GetVersion();
		aoe::EntityId id = entity.GetId();
		return std::hash<aoe::Version>()(version) ^ std::hash<aoe::EntityId>()(id);
	}
};

size_t SumOfAp(size_t a_1, size_t a_n, size_t n);

TEST(WorldTests, Add_AddComponentToExistedEntity_EntityHasComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	bool result = world.Has<size_t>(entity);

	ASSERT_TRUE(result);
}

TEST(WorldTests, Add_AddComponentToNotExistedEntity_Failure) {
	aoe::World world;
	aoe::Entity entity(0);

	ASSERT_DEATH(world.Add<size_t>(entity), "");
}

TEST(WorldTests, Get_DoNotAddAndGetComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	aoe::ComponentHandler<size_t> component = world.Get<size_t>(entity);

	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, Get_AddAndGetComponent_EntityHasComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	aoe::ComponentHandler<size_t> component = world.Get<size_t>(entity);
	
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Remove_RemoveAddedComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	world.Remove<size_t>(entity);
	bool result = world.Has<size_t>(entity);

	ASSERT_FALSE(result);
}

TEST(WorldTests, IsValid_CheckIsNotExistedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity(0);

	bool result = world.IsValid(entity);

	ASSERT_FALSE(result);
}

TEST(WorldTests, IsValid_CheckIsExistedEntityValid_True) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	bool result = world.IsValid(entity);

	ASSERT_TRUE(result);
}

TEST(WorldTests, IsValid_CheckIsRemovedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Destroy(entity);
	world.Validate();
	bool result = world.IsValid(entity);

	ASSERT_FALSE(result);
}

TEST(WorldTests, Destroy_CreateAndDestroyEntities_AllCreatedEntitiesAreUnique) {
	size_t max_alive_entities_count = 10;
	size_t recreated_entities_count = SumOfAp(1, max_alive_entities_count, max_alive_entities_count);
	size_t unique_entities_count = recreated_entities_count + max_alive_entities_count;

	std::unordered_set<aoe::Entity, EntityHashFuction> unique_entities;
	std::vector<aoe::Entity> entities;
	aoe::World world;

	for (size_t count = 0; count < max_alive_entities_count; ++count) {
		aoe::Entity entity = world.Create();
		entities.push_back(entity);
		unique_entities.insert(entity);
	}

	for (size_t to_remove = 1; to_remove <= max_alive_entities_count; ++to_remove) {
		for (size_t count = 1; count <= to_remove; ++count) {
			world.Destroy(entities.back());
			entities.pop_back();
		}

		world.Validate();

		while (entities.size() < max_alive_entities_count) {
			aoe::Entity entity = world.Create();
			entities.push_back(entity);
			unique_entities.insert(entity);
		}
	}

	ASSERT_TRUE(unique_entities.size() == unique_entities_count);
}

TEST(WorldTests, ForEach_IterateOverMatchedEntities_AllMatchedEntitiesIterated) {
	size_t entities_count = 10;
	std::unordered_set<aoe::Entity, EntityHashFuction> expected_entities;
	aoe::World world;

	for (size_t count = 0; count < entities_count; ++count) {
		aoe::Entity entity = world.Create();

		if (count % 2 == 0) {
			world.Add<TestComponentA>(entity);
			world.Add<TestComponentB>(entity);
			expected_entities.insert(entity);
		} else {
			world.Add<TestComponentA>(entity);
			world.Add<TestComponentC>(entity);
		}
	}

	world.ForEach<TestComponentA, TestComponentB>([&](auto entity, auto component_a, auto component_b) {
		expected_entities.erase(entity);
	});

	ASSERT_TRUE(expected_entities.empty());
}

size_t SumOfAp(size_t a_1, size_t a_n, size_t n) {
	return (a_1 + a_n) * n / 2;
}

} // ecs_tests
} // aoe_tests