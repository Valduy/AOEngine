#include <map>
#include <unordered_map>
#include <chrono>

#include "Preregistrator.h"
#include "Test1.h"
#include "Test2.h"
#include "../ECS/Component.h"
#include "../ECS/Entity.h"
#include "entt.hpp"

class TestComponentA : public aoe::Component<TestComponentA> {
AOE_REFLECTION_BEGIN(TestComponentA)
AOE_REFLECTION_END()

public:
	size_t Number;
};

class TestComponentB : public aoe::Component<TestComponentB> {
AOE_REFLECTION_BEGIN(TestComponentB)
AOE_REFLECTION_END()

public:
	float X;
	float Y;
	float Z;
};

class TestComponentC : public aoe::Component<TestComponentC> {
AOE_REFLECTION_BEGIN(TestComponentC)
AOE_REFLECTION_END()

public:
	char Token;
};

class Position : public aoe::Component<Position>
{
public:
	float x;
	float y;
};

class TestComponentD : public aoe::Component<TestComponentD> {
AOE_REFLECTION_BEGIN(TestComponentD)
AOE_REFLECTION_END()

public:
	float Epsilon;
};

class TestComponentE : public aoe::Component<TestComponentE> {
AOE_REFLECTION_BEGIN(TestComponentE)
AOE_REFLECTION_END()
};

class TestComponentF : public aoe::Component<TestComponentF> {
AOE_REFLECTION_BEGIN(TestComponentF)
AOE_REFLECTION_END()
};

class TestComponentG : public aoe::Component<TestComponentG> {
AOE_REFLECTION_BEGIN(TestComponentG)
AOE_REFLECTION_END()
};

class TestComponentH : public aoe::Component<TestComponentH> {
AOE_REFLECTION_BEGIN(TestComponentH)
AOE_REFLECTION_END()
};

// Archetype
// Family
// World

int main() {
	for (size_t ttt = 0; ttt < 1; ++ttt) {
		aoe::Entity entity;

		entity.Add<TestComponentH>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentG>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentF>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentE>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentD>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentC>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentB>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Add<TestComponentA>();
		std::cout << "\nFrom main" << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		// My remove logic somehow corrupt heap( (Compress??)

		entity.Remove<TestComponentE>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentE>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentC>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentC>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentD>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentD>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentD>();

		auto component_a = entity.Get<TestComponentA>();
		component_a->Number = 15;
		std::cout << "From component: " << component_a->Number << std::endl;

		entity.Remove<TestComponentF>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentF>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentB>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentB>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentG>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentG>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentH>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentH>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		entity.Remove<TestComponentA>();
		std::cout << "\nFrom main after remove component: " << aoe::Identifier::GetTypeId<TestComponentA>() << std::endl;
		entity.DebugPrint();
		std::cout << std::endl;

		// pseudotest
		entt::registry registry;
		const auto entt = registry.create();
		registry.emplace<TestComponentA>(entt);
		registry.emplace<TestComponentB>(entt);
		registry.emplace<TestComponentC>(entt);
		registry.emplace<TestComponentD>(entt);
		registry.emplace<TestComponentE>(entt);
		registry.emplace<TestComponentF>(entt);
		registry.emplace<TestComponentG>(entt);
		registry.emplace<TestComponentH>(entt);

		std::vector<std::function<void()>> enttccessors{
			[&]() {registry.get<TestComponentA>(entt); },
			[&]() {registry.get<TestComponentB>(entt); },
			[&]() {registry.get<TestComponentC>(entt); },
			[&]() {registry.get<TestComponentD>(entt); },
			[&]() {registry.get<TestComponentE>(entt); },
			[&]() {registry.get<TestComponentF>(entt); },
			[&]() {registry.get<TestComponentG>(entt); },
			[&]() {registry.get<TestComponentH>(entt); },
		};

		entity.Add<TestComponentA>();
		entity.Add<TestComponentB>();
		entity.Add<TestComponentC>();
		entity.Add<TestComponentD>();
		entity.Add<TestComponentE>();
		entity.Add<TestComponentF>();
		entity.Add<TestComponentG>();
		entity.Add<TestComponentH>();

		std::vector<std::function<void()>> accessors{
			[&]() {entity.Get<TestComponentA>(); },
			[&]() {entity.Get<TestComponentB>(); },
			[&]() {entity.Get<TestComponentC>(); },
			[&]() {entity.Get<TestComponentD>(); },
			[&]() {entity.Get<TestComponentE>(); },
			[&]() {entity.Get<TestComponentF>(); },
			[&]() {entity.Get<TestComponentG>(); },
			[&]() {entity.Get<TestComponentH>(); },
		};

		std::map<size_t, aoe::IComponent*> map_entity;
		map_entity[0] = new TestComponentA();
		map_entity[1] = new TestComponentB();
		map_entity[2] = new TestComponentC();
		map_entity[3] = new TestComponentD();
		map_entity[4] = new TestComponentE();
		map_entity[5] = new TestComponentF();
		map_entity[6] = new TestComponentG();
		map_entity[7] = new TestComponentH();

		std::map<size_t, aoe::IComponent*> umap_entity;
		umap_entity[0] = new TestComponentA();
		umap_entity[1] = new TestComponentB();
		umap_entity[2] = new TestComponentC();
		umap_entity[3] = new TestComponentD();
		umap_entity[4] = new TestComponentE();
		umap_entity[5] = new TestComponentF();
		umap_entity[6] = new TestComponentG();
		umap_entity[7] = new TestComponentH();

		size_t iterations = 2;
		size_t octet = 8;

		uint8_t signature = 0;
		auto start = std::chrono::high_resolution_clock::now();
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

		signature = 0;
		start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i) {
			uint8_t temp = signature;
			uint8_t mask = 1;

			for (size_t j = 0; j < octet; ++j) {
				if (signature & mask) {
					auto enttccessor = enttccessors[j];
					enttccessor();
				}

				mask <<= 1;
			}

			signature += 1;
		}

		stop = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Entt: " << duration.count() << std::endl;

		signature = 0;
		start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i) {
			uint8_t temp = signature;
			uint8_t mask = 1;

			for (size_t j = 0; j < octet; ++j) {
				if (signature & mask) {
					auto accessor = accessors[j];
					accessor();
				}

				mask <<= 1;
			}

			signature += 1;
		}

		stop = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Entity: " << duration.count() << std::endl;

		signature = 0;
		start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i) {
			uint8_t temp = signature;
			uint8_t mask = 1;

			for (size_t j = 0; j < octet; ++j) {
				if (signature & mask) {
					map_entity[j]->GetType();
				}

				mask <<= 1;
			}

			signature += 1;
		}

		stop = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Map: " << duration.count() << std::endl;

		signature = 0;

		start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i) {
			uint8_t temp = signature;
			uint8_t mask = 1;

			for (size_t j = 0; j < octet; ++j) {
				if (signature & mask) {
					umap_entity[j]->GetType();
				}

				mask <<= 1;
			}

			signature += 1;
		}

		stop = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
		std::cout << "Umap: " << duration.count() << std::endl;

		for (auto it : map_entity) {
			delete it.second;
		}

		for (auto it : umap_entity) {
			delete it.second;
		}
	}

	return 0;
}