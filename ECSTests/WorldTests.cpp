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
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	bool has = world.Has<size_t>(entity);
	auto component = world.Get<size_t>(entity);
	
	ASSERT_TRUE(has);
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Add_AddComponentToNotExistedEntity_Failure) {
	aoe::World world;
	aoe::Entity entity(0);

	ASSERT_DEATH(world.Add<size_t>(entity), "");
}

TEST(WorldTests, Get_DoNotAddAndGetComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	bool has = world.Has<size_t>(entity);
	auto component = world.Get<size_t>(entity);
	
	ASSERT_FALSE(has);
	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, Get_AddAndGetComponent_EntityHasComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	bool has = world.Has<size_t>(entity);
	auto component = world.Get<size_t>(entity);
	
	ASSERT_TRUE(has);
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Remove_RemoveAddedComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Add<size_t>(entity);
	world.Remove<size_t>(entity);
	bool has = world.Has<size_t>(entity);
	auto component = world.Get<size_t>(entity);

	ASSERT_FALSE(has);
	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, IsValid_CheckIsNotExistedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity(0);

	bool is_valid = world.IsValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(WorldTests, IsValid_CheckIsExistedEntityValid_True) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	bool is_valid = world.IsValid(entity);

	ASSERT_TRUE(is_valid);
}

TEST(WorldTests, IsValid_CheckIsRemovedEntityValid_False) {
	aoe::World world;
	aoe::Entity entity = world.Create();

	world.Destroy(entity);
	world.Validate();
	bool is_valid = world.IsValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(WorldTests, ForEach_IterateOverMatchedEntities_AllMatchedEntitiesIterated) {
	size_t entities_count = 10;
	std::unordered_set<aoe::Entity> expected_entities;
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

TEST(WorldTests, Iterator_IterateOverMatchedEntities_AllMatchedEntitiesIterated) {
	size_t entities_count = 10;
	std::unordered_set<aoe::Entity> expected_entities;
	aoe::World world;

	for (size_t count = 0; count < entities_count; ++count) {
		aoe::Entity entity = world.Create();

		if (count % 2 == 0) {
			world.Add<TestComponentA>(entity);
			world.Add<TestComponentB>(entity);
			expected_entities.insert(entity);
		}
		else {
			world.Add<TestComponentA>(entity);
			world.Add<TestComponentC>(entity);
		}
	}

	for (aoe::Entity entity : world.GetFilter<TestComponentA, TestComponentB>()) {
		expected_entities.erase(entity);
	}

	ASSERT_TRUE(expected_entities.empty());
}

} // ecs_tests
} // aoe_tests