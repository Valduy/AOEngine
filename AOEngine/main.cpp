#include <map>
#include <unordered_map>
#include <chrono>

#include "Preregistrator.h"
#include "Test1.h"
#include "Test2.h"
#include "../ECS/World.h"
#include "entt.hpp"

class TestComponentA {
AOE_REFLECTION_BEGIN(TestComponentA)
AOE_REFLECTION_END()

public:
	TestComponentA(size_t number)
		: Number(number)
	{}

	size_t Number;
};

class TestComponentB {
AOE_REFLECTION_BEGIN(TestComponentB)
AOE_REFLECTION_END()

public:
	float X;
	float Y;
	float Z;
};

class TestComponentC {
AOE_REFLECTION_BEGIN(TestComponentC)
AOE_REFLECTION_END()

public:
	char Token;
};

class Position
{
public:
	float x;
	float y;
};

class TestComponentD {
AOE_REFLECTION_BEGIN(TestComponentD)
AOE_REFLECTION_END()

public:
	float Epsilon;
};

class TestComponentE {
AOE_REFLECTION_BEGIN(TestComponentE)
AOE_REFLECTION_END()
};

class TestComponentF {
AOE_REFLECTION_BEGIN(TestComponentF)
AOE_REFLECTION_END()
};

class TestComponentG {
AOE_REFLECTION_BEGIN(TestComponentG)
AOE_REFLECTION_END()
};

class TestComponentH {
AOE_REFLECTION_BEGIN(TestComponentH)
AOE_REFLECTION_END()
};

// Archetype
// Family
// World

void PoolManualTest() {
	aoe::Pool<TestComponentA> pool;
	aoe::EntityId entity0 = 0;
	aoe::EntityId entity1 = 1;
	aoe::EntityId entity2 = 2;
	aoe::EntityId entity3 = 3;

	pool.Add(entity0, static_cast<size_t>(0));
	pool.Add(entity1, static_cast<size_t>(1));
	pool.Add(entity2, static_cast<size_t>(2));
	pool.Add(entity3, static_cast<size_t>(3));

	auto hd0 = pool.Get(entity0);
	auto hd1 = pool.Get(entity1);
	auto hd2 = pool.Get(entity2);
	auto hd3 = pool.Get(entity3);

	pool.Remove(entity0);
	pool.Remove(entity2);

	hd0 = pool.Get(entity0);
	hd1 = pool.Get(entity1);
	hd2 = pool.Get(entity2);
	hd3 = pool.Get(entity3);
}

void PoolCombinationsTest() {
	aoe::Pool<TestComponentA> pool;
	size_t octet = 8;

	for (uint8_t signature = 1; signature < std::numeric_limits<uint8_t>::max(); ++signature) {
		uint8_t reference = 1;

		for (size_t pos = 0; pos < octet; ++pos) {
			if (signature & reference) {
				aoe::EntityId entity = pos;
				pool.Add(entity, pos);
			}

			reference <<= 1;
		}

		reference = 1;

		for (size_t pos = 0; pos < octet; ++pos) {
			aoe::EntityId entity = pos;

			if (signature & reference) {
				assert(pool.Has(entity));
				assert(pool.Get(entity)->Number == pos);
			} else {
				assert(!pool.Has(entity));
				assert(pool.Get(entity) == nullptr);
			}

			reference <<= 1;
		}

		for (size_t pos = 0; pos < octet; ++pos) {
			aoe::EntityId entity = pos;
			pool.Remove(entity);
		}
	}
}

void TestPool() {
	PoolManualTest();
	PoolCombinationsTest();
}

void TestWorld() {
	aoe::World world;
	aoe::Entity e0 = world.Create();
	aoe::Entity e1 = world.Create();
	aoe::Entity e2 = world.Create();
	aoe::Entity e3 = world.Create();

	world.Destroy(e1);
	world.Validate();

	world.Add<TestComponentA>(e0, 0);
	world.Add<TestComponentA>(e2, 0);

	world.ForEach<TestComponentA>([&](auto ent, auto ca) {
		auto t = 0;
		//auto v = ent.version;
	});
}

int main() {
	TestPool();
	TestWorld();

	entt::registry registry;
	auto entity_0 = registry.create();
	auto version_0 = registry.current(entity_0);
	registry.destroy(entity_0);
	version_0 = registry.current(entity_0);
	auto entity_1 = registry.create();
	auto version_1 = registry.current(entity_1);
	registry.destroy(entity_1);

	return 0;
}