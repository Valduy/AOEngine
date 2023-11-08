#pragma once

#include <vector>

#include "Field.h"
#include "TypeName.h"

namespace aoe {

class TypeSettings {
public:
	std::vector<const Field*> fields;
	std::vector<const Type*> base_classes;
	std::function<void* ()> constructor_;

	TypeSettings() {
		constructor_ = []() {
			assert(false && "There is no registered constructor for this type.");
			return nullptr;
		};
	}

	TypeSettings& AddField(const  Field* field) {
		fields.push_back(field);
		return *this;
	}

	TypeSettings& AddBaseClass(const Type* type) {
		base_classes.push_back(type);
		return *this;
	}

	TypeSettings& SetConstructor(std::function<void* ()> constructor) {
		constructor_ = constructor;
		return *this;
	}

	template<typename T>
	const Type* Register() {
		std::string name(TypeName<T>());
		auto type_id = Identifier::GetTypeId<T>();
		auto* type = new Type(type_id, name, fields, base_classes, constructor_);
		Reflector::Register<T>(type);
		return type;
	}
};

} // namespace aoe