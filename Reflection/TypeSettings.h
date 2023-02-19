#pragma once

#include <vector>

#include "Field.h"
#include "TypeName.h"

namespace aoe {

class TypeSettings {
public:
	std::vector<const Field*> fields;
	std::vector<const Type*> base_classes;

	TypeSettings& AddField(const  Field* field) {
		fields.push_back(field);
		return *this;
	}

	TypeSettings& AddBaseClass(const Type* type) {
		base_classes.push_back(type);
		return *this;
	}

	template<typename T>
	const Type& Register() {
		std::string name(TypeName<T>());
		auto* type = new Type(Identifier::GetTypeId<T>(), name, fields, base_classes);
		return Reflector::Register<T>(type);
	}
};

} // namespace aoe