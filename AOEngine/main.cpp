#include "Preregistrator.h"
#include "Test1.h"
#include "Test2.h"
#include "../ECS/Component.h"

//AOE_REFLECTION_OUTER_BEGIN(size_t)
//AOE_REFLECTION_OUTER_END()

AOE_REFLECTION_OUTER_BEGIN(void*)
AOE_REFLECTION_OUTER_END()

AOE_REFLECTION_OUTER_BEGIN(std::vector<int>)
AOE_REFLECTION_OUTER_END()

class Base {
public:
	size_t Num;
};

AOE_REFLECTION_OUTER_BEGIN(Base)
AOE_REFLECTION_FIELD(Num)
AOE_REFLECTION_OUTER_END()

class Test : public Base {
AOE_REFLECTION_BEGIN(Test)
AOE_REFLECTION_BASE(Base)
AOE_REFLECTION_FIELD(Number)
AOE_REFLECTION_FIELD(Ptr)
AOE_REFLECTION_FIELD(Numbers)
AOE_REFLECTION_FIELD(Other)
AOE_REFLECTION_END()

public:
	size_t Number;	
	void* Ptr;
	std::vector<int> Numbers;

	std::function<void*(void*)> test = AOE_FIELD_GETTER(Test, Other);

private:
	size_t Other = 1;
};

class TestComponent : public aoe::Component<TestComponent> {
AOE_REFLECTION_BEGIN(TestComponent)
AOE_REFLECTION_END()
};

int main() {
	Test1 t1;
	Test2 t2;

	auto id = aoe::Identifier::GetTypeId<size_t>();
	auto naname = aoe::TypeName<size_t>();

	const auto& type1 = aoe::Reflector::GetType<size_t>();
	const auto& type2 = aoe::Reflector::GetType<Test>();

	Test test;
	test.Number = 10;
	test.Numbers.push_back(1);
	test.Numbers.push_back(2);

	size_t n = 15;
	size_t number = type2->GetFields()[0]->GetValue<size_t>(&test);
	type2->GetFields()[0]->SetValue<size_t>(&test, 6);

	void* ptr = type2->GetFields()[1]->GetValue<void*>(&test);
	void* other_ptr = test.Ptr;
	type2->GetFields()[1]->SetValue(&test, nullptr);

	std::vector<int> numbers = type2->GetFields()[2]->GetValue<std::vector<int>>(&test);
	numbers.push_back(3);
	type2->GetFields()[2]->SetValue<std::vector<int>>(&test, numbers);

	auto other = *static_cast<size_t*>(test.test(&test));

	auto& testest = type2->GetBaseClasses();

	TestComponent tc;
	auto type = tc.GetType();

	return 0;
}