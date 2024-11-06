#include <iostream>

#include "../ECS/SparseMap.h"

struct TestComponent {
	TestComponent() {
		std::cout << "TestComponent()\n";
	}

	TestComponent(const TestComponent& other) {
		std::cout << "TestComponent(const TestComponent&)\n";
	}

	TestComponent(TestComponent&& other) noexcept {
		std::cout << "TestComponent(TestComponent&&)\n";
	}

	TestComponent& operator=(const TestComponent&) {
		std::cout << "operator=(const TestComponent&)\n";
		return *this;
	}

	TestComponent& operator=(TestComponent&&) noexcept {
		std::cout << "operator=(TestComponent&&)\n";
		return *this;
	}
};

int main() {
	aoe::SparseMap<TestComponent>::Id id = 0;
	aoe::SparseMap<TestComponent> map;

	TestComponent component;
	map.Add(id, component);
	std::cout << std::endl;

	map.Remove(id);
	map.Emplace(id);

	return 0;
}