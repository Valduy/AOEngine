#include "pch.h"

#include "../ECS/Pool.h"

namespace aoe_tests {
namespace ecs_tests {

TEST(PoolTests, Get_PoolIsEmpty_Nullptr) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	size_t* component = pool.Get(entity);

	ASSERT_EQ(component, nullptr);
}

TEST(PoolTests, Get_EntityIsAddedToPool_Component) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);
	size_t value = 42;

	pool.Add(entity, value);
	size_t* component = pool.Get(entity);

	ASSERT_EQ(*component, value);
}

TEST(PoolTests, Get_EntityIsNotAddedToPool_Nullptr) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity_0(0);
	aoe::Entity entity_1(1);
	
	pool.Add(entity_0);
	size_t* component = pool.Get(entity_1);

	ASSERT_EQ(component, nullptr);
}

TEST(PoolTests, Get_EntityIsRemovedFromPool_Nullptr) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	pool.Add(entity);
	pool.Remove(entity);
	size_t* component = pool.Get(entity);

	ASSERT_EQ(component, nullptr);
}

TEST(PoolTests, Has_PoolIsEmpty_False) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	bool result = pool.Has(entity);

	ASSERT_FALSE(result);
}

TEST(PoolTests, Has_EntityIsAddedToPool_True) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	pool.Add(entity);
	bool result = pool.Has(entity);

	ASSERT_TRUE(result);
}

TEST(PoolTests, Has_EntityIsNotAddedToPool_False) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity_0(0);
	aoe::Entity entity_1(1);
	
	pool.Add(entity_0);
	bool result = pool.Has(entity_1);

	ASSERT_FALSE(result);
}

TEST(PoolTests, Has_EntityIsRemovedFromPool_False) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	pool.Add(entity);
	pool.Remove(entity);
	bool result = pool.Has(entity);

	ASSERT_FALSE(result);
}

TEST(PoolTests, Remove_PoolIsEmpty_HasNotEntity) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity(0);

	pool.Remove(entity);
	bool result = !pool.Has(entity);

	ASSERT_TRUE(result);
}

TEST(PoolTests, Remove_EntityIsNotAddedToPool_HasNotRemovedEntity) {
	aoe::Pool<size_t> pool;
	aoe::Entity entity_0(0);
	aoe::Entity entity_1(1);

	pool.Add(entity_0);
	pool.Remove(entity_1);
	bool result = pool.Has(entity_0) && !pool.Has(entity_1);

	ASSERT_TRUE(result);
}

TEST(PoolTests, Remove_MultipleEntitiesInPool_HasNotRemovedEntity) {
	aoe::EntityId end = 10;
	
	for (aoe::EntityId to_remove = 0; to_remove < end; ++to_remove) {
		aoe::Pool<size_t> pool;

		for (aoe::EntityId entity_id = 0; entity_id < end; ++entity_id) {
			pool.Add({ entity_id });
		}

		pool.Remove({ to_remove });

		for (aoe::EntityId entity = 0; entity < end; ++entity) {
			if (entity == to_remove) {
				ASSERT_FALSE(pool.Has({ entity }));
			} else {
				ASSERT_TRUE(pool.Has({ entity }));
			}
		}
	}
}

TEST(PoolTests, Remove_AddAndRemoveMultipleEntities_HasNotRemovedEntity) {
	uint8_t signature = std::numeric_limits<uint8_t>::max();
	uint8_t mask = 1;
	size_t end = 8;

	for (; signature > 0; --signature) {
		aoe::Pool<size_t> pool;

		for (aoe::EntityId entity = 0; entity < end; ++entity) {
			pool.Add({ entity });
		}

		for (aoe::EntityId entity = 0; entity < end; ++entity) {
			uint8_t sign = (signature >> entity) & mask;

			if (!sign) {
				pool.Remove({ entity });
			}
		}

		for (aoe::EntityId entity = 0; entity < end; ++entity) {
			uint8_t sign = (signature >> entity) & mask;

			if (!sign) {
				ASSERT_FALSE(pool.Has({ entity }));
			} else {
				ASSERT_TRUE(pool.Has({ entity }));
			}
		}
	}
}

} // ecs_tests
} // aoe_tests