#include "pch.h"

#include "../Reflection/Reflection.h"

namespace aoe_tests {
namespace reflector_tests {

class MyTest1 {
AOE_REFLECTION_BEGIN(MyTest1)
AOE_REFLECTION_END()
};

class MyTest2 {};

TEST(ReflectorTests, GetType_TypeIsRegistered_WillNotFailure) {
	const auto& type = aoe::Reflector::GetType<MyTest1>();

	ASSERT_TRUE(true);
}

TEST(ReflectorTests, GetType_TypeIsNotRegistered_WillFailure) {
	ASSERT_DEATH(aoe::Reflector::GetType<MyTest2>(), "");
}

TEST(ReflectorTests, Register_TypeIsAlreadyRegistered_WillFailure) {
	aoe::TypeSettings type_settings;

	ASSERT_DEATH(type_settings.Register<MyTest1>(), "");
}

} // namespace reflection_tests
} // aoe_tests