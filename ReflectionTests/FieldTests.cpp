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

TEST(FieldTests, GetValue_SetObjectFieldValue_ValueEqualToObjectFieldValue) {
	TestClass1 test;
	test.Number = 1;

	auto& type = aoe::Reflector::GetType<TestClass1>();
	auto& fields = type.GetFields();
	auto predicate = [](const aoe::Field* f) { return f->GetName() == "Number"; };
	auto number_field_it = std::find_if(fields.begin(), fields.end(), predicate);
	auto number_field = *number_field_it;

	size_t number = number_field->GetValue<size_t>(&test);

	ASSERT_EQ(test.Number, number);
}

} // namespace field_tests
} // namespace aoe_tests