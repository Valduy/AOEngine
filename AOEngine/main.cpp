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

	return 0;
}