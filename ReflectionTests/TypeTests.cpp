#include "pch.h"

// test get base classes
#include "pch.h"

#include "Preregistration.h"

// test get fields
// test instantiate

namespace aoe_tests {
namespace type_tests {

class TestClassParentA {
AOE_REFLECTION_BEGIN(TestClassParentA)
AOE_REFLECTION_END()
};

class TestClassParentB {
AOE_REFLECTION_BEGIN(TestClassParentB)
AOE_REFLECTION_FIELD(ParentNumber)
AOE_REFLECTION_END()

private:
	size_t ParentNumber;
};

class TestClassChildren : public TestClassParentA, public TestClassParentB {
AOE_REFLECTION_BEGIN(TestClassChildren)
AOE_REFLECTION_FIELD(ChildrenNumber1)
AOE_REFLECTION_FIELD(ChildrenNumber2)
AOE_REFLECTION_END()

public:
	bool Foo() {
		return true;
	}

private:
	size_t ChildrenNumber1;
	size_t ChildrenNumber2;
};

TEST(TypeTests, Instantiate_) {
	auto& type = aoe::Reflector::GetType<TestClassChildren>();
	auto* instance = type.Instantiate<TestClassChildren>();
	
	ASSERT_NO_FATAL_FAILURE(instance->Foo());

	delete instance;
}

} // namespace type_tests
} // namespace aoe_tests