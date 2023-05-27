#include "pch.h"

#include <algorithm>

#include "Preregistration.h"

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

	const aoe::Type* type = aoe::Reflector::GetType<TestClass1>();
	const aoe::Field* number_field = type->GetFieldByName("Number");
	size_t number = number_field->GetValue<size_t>(&test);

	ASSERT_EQ(test.Number, number);
}

TEST(FieldTests, SetValue_SetValueViaReflectionField_ObjectFieldValueIsEuivalentToSetted) {
	TestClass2 test;
	std::vector<size_t> value = { 1, 2, 3, 4 };

	const aoe::Type* type = aoe::Reflector::GetType<TestClass2>();
	const aoe::Field* numbers_field = type->GetFieldByName("Numbers");
	numbers_field->SetValue(&test, value);
	
	ASSERT_NE(value.data(), test.Numbers.data());
	ASSERT_TRUE(std::equal(test.Numbers.begin(), test.Numbers.end(), value.begin()));
}

} // namespace field_tests
} // namespace aoe_tests