#pragma once

#include <vector>
#include <algorithm>

#include "Identifier.h"
#include "Field.h"

namespace aoe {

class Type {
public:
	Type(
		TypeId type_id,
		std::string name,
		std::vector<const Field*> fields,
		std::vector<const Type*> base_classes)
		: type_id_(type_id)
		, name_(std::move(name))
		, fields_(std::move(fields))
		, base_classes_(std::move(base_classes))
	{}

	Type(const Type&) = delete;
	Type(Type&&) = delete;

	TypeId GetTypeId() const {
		return type_id_;
	}

	const std::string& GetName() const {
		return name_;
	}

	const std::vector<const Type*>& GetBaseClasses() const {
		return base_classes_;
	}

	const std::vector<const Field*>& GetFields() const {
		return fields_;
	}

	const Field* GetFieldByName(const std::string& name) {
		auto it = std::find_if(fields_.begin(), fields_.end(), [&name](const auto* field) {
			return field->GetName() == name;
		});

		return *it;
	}

private:
	TypeId type_id_;
	std::string name_;
	std::vector<const Field*> fields_;
	std::vector<const Type*> base_classes_;
};

} // namespace aoe