#pragma once

#include <functional>
#include <string>

namespace aoe {

class Type;

class Field {
public:
	Field(
		std::string name,
		const Type& type,
		std::function<void* (void*)> getter,
		std::function<void(void*, void*)> setter)
		: name_(std::move(name))
		, type_(type)
		, getter_(getter)
		, setter_(setter)
	{}

	void* GetValue(void* object) const {
		return getter_(object);
	}

	template<typename T>
	T GetValue(void* object) const {
		auto value = GetValue(object);
		return *static_cast<T*>(value);
	}

	template<typename T>
	void SetValue(void* object, T* value) const {
		setter_(object, value);
	}

	template<typename T>
	void SetValue(void* object, T value) const {
		setter_(object, &value);
	}

private:
	std::string name_;
	const Type& type_;
	std::function<void* (void*)> getter_;
	std::function<void(void*, void*)> setter_;
};

} // namespace aoe