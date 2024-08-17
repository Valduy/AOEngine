#include "pch.h"

#include "../ECS/SparseMap.h"

namespace aoe_tests {
namespace ecs_tests {

TEST(SparseMapTests, Get_GetAddedId_AddedId) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;
	const size_t value = 42;

	sparse_map.Add(id, value);
	size_t result = sparse_map.Get(id);

	ASSERT_EQ(result, value);
}

TEST(SparseMapTests, Get_GetNotAddedId_Failure) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	ASSERT_DEATH(sparse_map.Get(id), "");
}

TEST(SparseMapTests, Get_GetRemovedId_Failure) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	sparse_map.Add(id);
	sparse_map.Remove(id);

	ASSERT_DEATH(sparse_map.Get(id), "");
}

TEST(SparseMapTests, Has_SetIsEmpty_False) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	bool has = sparse_map.Has(id);

	ASSERT_FALSE(has);
}

TEST(SparseMapTests, Has_CheckAddedId_True) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	sparse_map.Add(id);
	bool has = sparse_map.Has(id);

	ASSERT_TRUE(has);
}

TEST(SparseMapTests, Has_AddOneIdAndCheckOther_False) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id_to_add = 0;
	const aoe::SparseMap<size_t>::Id id_to_check = 1;

	sparse_map.Add(id_to_add);
	bool has = sparse_map.Has(id_to_check);

	ASSERT_FALSE(has);
}

TEST(SparseMapTests, Has_AddAndRemoveId_False) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	sparse_map.Add(id);
	sparse_map.Remove(id);
	bool has = sparse_map.Has(id);

	ASSERT_FALSE(has);
}

TEST(SparseMapTests, Remove_SetIsEmpty_HasNotRemovedId) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id = 0;

	sparse_map.Remove(id);
	bool has = sparse_map.Has(id);

	ASSERT_FALSE(id);
}

TEST(SparseMapTests, Remove_IdIsNotAddedToSet_HasNotRemovedId) {
	aoe::SparseMap<size_t> sparse_map;
	const aoe::SparseMap<size_t>::Id id_to_add = 0;
	const aoe::SparseMap<size_t>::Id id_to_remove = 1;

	sparse_map.Add(id_to_add);
	sparse_map.Remove(id_to_remove);
	bool result = sparse_map.Has(id_to_add) && !sparse_map.Has(id_to_remove);

	ASSERT_TRUE(result);
}

TEST(SparseMapTests, Remove_MultipleIdsInPool_HasNotRemovedId) {
	const aoe::SparseMap<size_t>::Id end = 10;

	for (aoe::SparseMap<size_t>::Id id_to_remove = 0; id_to_remove < end; ++id_to_remove) {
		aoe::SparseMap<size_t> sparse_map;

		for (aoe::SparseMap<size_t>::Id id_to_add = 0; id_to_add < end; ++id_to_add) {
			sparse_map.Add(id_to_add, id_to_add);
		}

		sparse_map.Remove(id_to_remove);

		for (aoe::SparseMap<size_t>::Id id = 0; id < end; ++id) {
			if (id == id_to_remove) {
				ASSERT_FALSE(sparse_map.Has(id));
			}
			else {
				ASSERT_TRUE(sparse_map.Has(id));
				ASSERT_EQ(sparse_map.Get(id), id);
			}
		}
	}
}

TEST(SparseMapTests, Remove_AddAndRemoveMultipleIds_HasNotRemovedIds) {
	uint8_t signature = std::numeric_limits<uint8_t>::max();

	const uint8_t mask = 1;
	const aoe::SparseMap<size_t>::Id end = 8;

	for (; signature > 0; --signature) {
		aoe::SparseMap<size_t> sparse_map;

		for (aoe::SparseMap<size_t>::Id id = 0; id < end; ++id) {
			sparse_map.Add(id, id);
		}

		for (aoe::SparseMap<size_t>::Id id = 0; id < end; ++id) {
			const uint8_t sign = (signature >> id) & mask;

			if (sign == 0) {
				sparse_map.Remove(id);
			}
		}

		for (aoe::SparseMap<size_t>::Id id = 0; id < end; ++id) {
			const uint8_t sign = (signature >> id) & mask;

			if (sign == 0) {
				ASSERT_FALSE(sparse_map.Has(id));
			}
			else {
				ASSERT_TRUE(sparse_map.Has(id));
				ASSERT_EQ(sparse_map.Get(id), id);
			}
		}
	}
}

} // core_tests
} // aoe_tests