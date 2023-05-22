#pragma once

#include <functional>

#include "Reflector.h"
#include "TypeSettings.h"
#include "MetaHolder.h"
#include "Type.h"

#define AOE_FIELD_TYPE(type, field)\
	aoe::Reflector::GetType<decltype(SelfType::field)>()\

#define AOE_FIELD_GETTER(type, field)\
	[](void* object) { return &(static_cast<type*>(object))->field; }\

#define AOE_FIELD_SETTER(type, field)\
	[](void* object, void* value) { \
		auto* concrete_object = static_cast<type*>(object);\
		auto* concrete_value = static_cast<decltype(type::field)*>(value);\
		concrete_object->field = *concrete_value;\
	}\

#define AOE_TYPE_CONSTRUCTOR(type)\
	[](){ return new type(); }\

#define AOE_REFLECTION_BEGIN(type)\
	private:\
		class Meta {\
		private:\
			using SelfType = type;\
			static const aoe::Type& GetType() {\
				aoe::TypeSettings type_settings;\
				type_settings.SetConstructor(AOE_TYPE_CONSTRUCTOR(SelfType));\

#define AOE_REFLECTION_FIELD(field)\
				type_settings.AddField(new aoe::Field {\
					#field,	AOE_FIELD_TYPE(SelfType, field), AOE_FIELD_GETTER(SelfType, field), AOE_FIELD_SETTER(SelfType, field)\
				});\

#define AOE_REFLECTION_BASE(type)\
				static_assert(std::is_convertible<SelfType*, type*>::value, "Derived must inherit Base as public");\
				type_settings.AddBaseClass(&aoe::Reflector::GetType<type>());\

#define AOE_REFLECTION_END()\
				return type_settings.Register<SelfType>();\
			}\
			static const inline aoe::Type& type_ = GetType();\
		};\

#define AOE_REFLECTION_OUTER_BEGIN(type)\
	namespace aoe {\
	template<>\
	class MetaHolder<type> {\
	AOE_REFLECTION_BEGIN(type)\

#define AOE_REFLECTION_OUTER_END()\
	AOE_REFLECTION_END()\
	};\
	}\
