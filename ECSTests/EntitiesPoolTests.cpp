#include "pch.h"

#include <unordered_set>
#include <deque>

#include "../ECS/EntitiesPool.h"

namespace aoe_tests {
namespace ecs_tests {

size_t SumOfAp(size_t a_1, size_t a_n, size_t n);

TEST(EntitiesPoolTests, IsValid_CheckIsNotExistedEntityValid_False) {
	aoe::EntitiesPool pool;
	aoe::Entity entity(0);

	bool is_valid = pool.IsValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(EntitiesPoolTests, IsValid_CheckIsExistedEntityValid_True) {
	aoe::EntitiesPool pool;
	aoe::Entity entity = pool.Create();

	bool is_valid = pool.IsValid(entity);

	ASSERT_TRUE(is_valid);
}

TEST(EntitiesPoolTests, IsValid_CheckIsRemovedEntityValid_False) {
	aoe::EntitiesPool pool;
	aoe::Entity entity = pool.Create();

	pool.Destroy(entity);
	bool is_valid = pool.IsValid(entity);

	ASSERT_FALSE(is_valid);
}

TEST(EntitiesPoolTests, Destroy_CreateAndDestroyEntities_AllCreatedEntitiesAreUnique) {
	const size_t max_alive_entities_count = 100;
	const size_t recreated_entities_count = SumOfAp(1, max_alive_entities_count, max_alive_entities_count);
	const size_t unique_entities_count = recreated_entities_count + max_alive_entities_count;

	std::unordered_set<aoe::Entity> unique_entities;
	std::vector<aoe::Entity> entities;
	aoe::EntitiesPool pool;

	for (size_t count = 0; count < max_alive_entities_count; ++count) {
		aoe::Entity entity = pool.Create();
		entities.push_back(entity);
		unique_entities.insert(entity);
	}

	for (size_t to_remove = 1; to_remove <= max_alive_entities_count; ++to_remove) {
		for (size_t count = 1; count <= to_remove; ++count) {
			pool.Destroy(entities.back());
			entities.pop_back();
		}

		while (entities.size() < max_alive_entities_count) {
			aoe::Entity entity = pool.Create();
			entities.push_back(entity);
			unique_entities.insert(entity);
		}

		std::reverse(entities.begin(), entities.end());
	}

	ASSERT_TRUE(unique_entities.size() == unique_entities_count);
}

TEST(EntitiesPoolTests, Iterator_IterateOverExistedEntities_AllExistedEntitiesIterated) {
	const size_t max_alive_entities_count = 100;

	std::unordered_set<aoe::Entity> removed_entities;
	std::deque<aoe::Entity> alive_entities;
	aoe::EntitiesPool pool;

	for (size_t i = 0; i < max_alive_entities_count; ++i) {
		aoe::Entity entity = pool.Create();
		
		if (i % 2 == 0) {
			alive_entities.push_back(entity);
		} else {
			alive_entities.push_front(entity);
		}
	}
	
	while (!alive_entities.empty()) {
		aoe::Entity removed = alive_entities.back();
		alive_entities.pop_back();

		pool.Destroy(removed);
		removed_entities.insert(removed);

		for (aoe::Entity entity : pool) {
			auto it = std::find(alive_entities.begin(), alive_entities.end(), entity);

			ASSERT_FALSE(removed_entities.contains(entity));
			ASSERT_TRUE(it != alive_entities.end());
		}
	}
}

size_t SumOfAp(size_t a_1, size_t a_n, size_t n) {
	return (a_1 + a_n) * n / 2;
}

} // namespace ecs_tests
} // namespace aoe_tests