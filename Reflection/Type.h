#pragma once

#include <vector>

#include "Identifier.h"
#include "Field.h"

namespace aoe {

class Type {
	friend class TypeSettings;
	
public:
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

private:
	TypeId type_id_;
	std::string name_;
	std::vector<const Field*> fields_;
	std::vector<const Type*> base_classes_;

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
};

} // namespace aoe