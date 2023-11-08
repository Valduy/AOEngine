#include "pch.h"

#include "../ECS/World.h"

// focus on version-valid-create-destroy-validate

namespace aoe_tests{
namespace ecs_tests {

TEST(WorldTests, Add_AddComponentToExistedEntity_EntityHasComponent) {
	aoe::World world;
	aoe::EntityId entity = world.Create();

	world.Add<size_t>(entity);
	bool result = world.Has<size_t>(entity);

	ASSERT_TRUE(result);
}

TEST(WorldTests, Add_AddComponentToNotExistedEntity_Failure) {
	aoe::World world;
	aoe::EntityId entity = 0;

	ASSERT_DEATH(world.Add<size_t>(entity), "");
}

TEST(WorldTests, Get_DoNotAddAndGetComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::EntityId entity = world.Create();

	aoe::ComponentHandler<size_t> component = world.Get<size_t>(entity);

	ASSERT_FALSE(component.IsValid());
}

TEST(WorldTests, Get_AddAndGetComponent_EntityHasComponent) {
	aoe::World world;
	aoe::EntityId entity = world.Create();

	world.Add<size_t>(entity);
	aoe::ComponentHandler<size_t> component = world.Get<size_t>(entity);
	
	ASSERT_TRUE(component.IsValid());
}

TEST(WorldTests, Remove_RemoveAddedComponent_EntityHasNotComponent) {
	aoe::World world;
	aoe::EntityId entity = world.Create();

	world.Add<size_t>(entity);
	world.Remove<size_t>(entity);
	bool result = world.Has<size_t>(entity);

	ASSERT_FALSE(result);
}

} // ecs_tests
} // aoe_tests