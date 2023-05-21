#include "pch.h"

#include <algorithm>

#include "Preregistration.h"

// For trivial data types get and set
// For complex data types get and set

namespace aoe_tests {
namespace field_tests {

class TestClass1 {
AOE_REFLECTION_BEGIN(TestClass1)
AOE_REFLECTION_FIELD(Number)
AOE_REFLECTION_END()

public:
	size_t Number;
};

class TestClass2 {
AOE_REFLECTION_BEGIN(TestClass2)
AOE_REFLECTION_FIELD(Numbers)
AOE_REFLECTION_END()

public:
	std::vector<size_t> Numbers;
};

TEST(FieldTests, GetValue_SetObjectFieldValue_ReflectionFieldValueEqualToObjectFieldValue) {
	TestClass1 test;
	test.Number = 1;

	auto& type = aoe::Reflector::GetType<TestClass1>();
	auto number_field = type.GetFieldByName("Number");
	size_t number = number_field->GetValue<size_t>(&test);

	ASSERT_EQ(test.Number, number);
}

TEST(FieldTests, SetValue_SetValueViaReflectionField_ObjectFieldValueIsEuivalentToSetted) {
	TestClass2 test;
	std::vector<size_t> value = { 1, 2, 3, 4 };

	auto& type = aoe::Reflector::GetType<TestClass2>();
	auto numbers_field = type.GetFieldByName("Numbers");
	numbers_field->SetValue(&test, value);
	
	ASSERT_NE(value.data(), test.Numbers.data());
	ASSERT_TRUE(std::equal(test.Numbers.begin(), test.Numbers.end(), value.begin()));
}

} // namespace field_tests
} // namespace aoe_tests