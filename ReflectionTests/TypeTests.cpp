#include "pch.h"

#include "pch.h"

#include "Preregistration.h"

namespace aoe_tests {
namespace type_tests {

class OtherTestClass {
AOE_REFLECTION_BEGIN(OtherTestClass)
AOE_REFLECTION_END()
};

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
AOE_REFLECTION_BASE(TestClassParentA)
AOE_REFLECTION_BASE(TestClassParentB)
AOE_REFLECTION_CONSTRUCTOR()
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

bool IsFieldsMatchWithNames(std::vector<const aoe::Field*> fields, std::vector<std::string> names);
bool IsBaseClassesMatchWithNames(std::vector<const aoe::Type*> types, std::vector<std::string> names);

TEST(TypeTests, GetFields_GetRegisteredFields_AllRegisteredFieldsPresent) {
	const aoe::Type* type = aoe::Reflector::GetType<TestClassChildren>();
	std::vector<std::string> names = { 
		"ChildrenNumber1", 
		"ChildrenNumber2",
	};

	auto fields = type->GetFields();

	ASSERT_TRUE(IsFieldsMatchWithNames(fields, names));
}

TEST(TypeTests, GetBaseClasses_GetRegisteredBaseClasses_AllBaseClassesPresent) {
	const aoe::Type* type = aoe::Reflector::GetType<TestClassChildren>();
	std::vector<std::string> names = { 
		std::string(aoe::TypeName<TestClassParentA>()), 
		std::string(aoe::TypeName<TestClassParentB>()),
	};
	
	auto types = type->GetBaseClasses();

	ASSERT_TRUE(IsBaseClassesMatchWithNames(types, names));
}

TEST(TypeTests, IsChildOf_RegisteredBaseClass_True) {
	const aoe::Type* children_type = aoe::Reflector::GetType<TestClassChildren>();
	const aoe::Type* base_type = aoe::Reflector::GetType<TestClassParentA>();

	ASSERT_TRUE(children_type->IsChildOf(base_type));
}

TEST(TypeTests, IsChildOf_NotBaseClass_False) {
	const aoe::Type* type = aoe::Reflector::GetType<TestClassChildren>();
	const aoe::Type* other_type = aoe::Reflector::GetType<OtherTestClass>();

	ASSERT_FALSE(type->IsChildOf(other_type));
}

TEST(TypeTests, Instantiate_InstantiateClassWithRegisteredByType_NoFailures) {
	const aoe::Type* type = aoe::Reflector::GetType<TestClassChildren>();
	TestClassChildren* instance = type->Instantiate<TestClassChildren>();
	
	ASSERT_NO_FATAL_FAILURE(instance->Foo());

	delete instance;
}

TEST(TypeTests, Instantiate_InstantiateClassWithoutRegisteredByType_Failure) {
	const aoe::Type* type = aoe::Reflector::GetType<TestClassParentA>();
	ASSERT_DEATH(type->Instantiate<TestClassParentA>(), "");
}

bool IsFieldsMatchWithNames(std::vector<const aoe::Field*> fields, std::vector<std::string> names) {
	if (fields.size() != names.size()) {
		return false;
	}
	
	for (size_t i = 0; i < fields.size(); ++i) {
		if (fields[i]->GetName() != names[i]) {
			return false;
		}
	}

	return true;
}

bool IsBaseClassesMatchWithNames(std::vector<const aoe::Type*> types, std::vector<std::string> names) {
	if (types.size() != names.size()) {
		return false;
	}

	for (size_t i = 0; i < types.size(); ++i) {
		if (types[i]->GetName() != names[i]) {
			return false;
		}
	}

	return true;
}

} // namespace type_tests
} // namespace aoe_tests