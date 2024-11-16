#include "pch.h"

#include <unordered_set>

#include "../ECS/World.h"

namespace aoe_tests{
namespace ecs_tests {

struct TestComponentA {};

struct TestComponentB {};

struct TestComponentC {};

TEST(WorldTests, Add_AddComponentToExistedEntity_EntityHasComponent) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	world.AddComponent<size_t>(entity);
	bool has = world.HasComponent<size_t>(entity);
	auto component = world.GetComponent<size_t>(entity);
	
	ASSERT_TRUE(has);
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Add_AddComponentToNotExistedEntity_Failure) {
	aoe::World world;
	aoe::Entity entity(0);

	ASSERT_DEATH(world.AddComponent<size_t>(entity), "");
}

TEST(WorldTests, Get_DoNotAddAndGetComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	bool has = world.HasComponent<size_t>(entity);
	auto component = world.GetComponent<size_t>(entity);
	
	ASSERT_FALSE(has);
	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, Get_AddAndGetComponent_EntityHasComponent) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	world.AddComponent<size_t>(entity);
	bool has = world.HasComponent<size_t>(entity);
	auto component = world.GetComponent<size_t>(entity);
	
	ASSERT_TRUE(has);
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Remove_RemoveAddedComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	world.AddComponent<size_t>(entity);
	world.RemoveComponent<size_t>(entity);
	bool has = world.HasComponent<size_t>(entity);
	auto component = world.GetComponent<size_t>(entity);

	ASSERT_FALSE(has);
	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, IsValid_CheckIsNotExistedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity(0);

	bool is_valid = world.IsEntityValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(WorldTests, IsValid_CheckIsExistedEntityValid_True) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	bool is_valid = world.IsEntityValid(entity);

	ASSERT_TRUE(is_valid);
}

TEST(WorldTests, IsValid_CheckIsRemovedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity = world.CreateEntity();

	world.DestroyEntity(entity);
	world.Validate();
	bool is_valid = world.IsEntityValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(WorldTests, ForEach_IterateOverMatchedEntities_AllMatchedEntitiesIterated) {
	size_t entities_count = 10;
	std::unordered_set<aoe::Entity> expected_entities;
	aoe::World world;

	for (size_t count = 0; count < entities_count; ++count) {
		aoe::Entity entity = world.CreateEntity();

		if (count % 2 == 0) {
			world.AddComponent<TestComponentA>(entity);
			world.AddComponent<TestComponentB>(entity);
			expected_entities.insert(entity);
		} else {
			world.AddComponent<TestComponentA>(entity);
			world.AddComponent<TestComponentC>(entity);
		}
	}

	world.ForEach<TestComponentA, TestComponentB>([&](auto entity, auto component_a, auto component_b) {
		expected_entities.erase(entity);
	});

	ASSERT_TRUE(expected_entities.empty());
}

TEST(WorldTests, Iterator_IterateOverMatchedEntities_AllMatchedEntitiesIterated) {
	size_t entities_count = 10;
	std::unordered_set<aoe::Entity> expected_entities;
	aoe::World world;

	for (size_t count = 0; count < entities_count; ++count) {
		aoe::Entity entity = world.CreateEntity();

		if (count % 2 == 0) {
			world.AddComponent<TestComponentA>(entity);
			world.AddComponent<TestComponentB>(entity);
			expected_entities.insert(entity);
		}
		else {
			world.AddComponent<TestComponentA>(entity);
			world.AddComponent<TestComponentC>(entity);
		}
	}

	for (aoe::Entity entity : world.FilterEntities<TestComponentA, TestComponentB>()) {
		expected_entities.erase(entity);
	}

	ASSERT_TRUE(expected_entities.empty());
}

} // ecs_tests
} // aoe_tests