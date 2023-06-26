#include "Preregistrator.h"
#include "Test1.h"
#include "Test2.h"
#include "../ECS/Component.h"
#include "../ECS/Entity.h"

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

int main() {
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

	return 0;
}