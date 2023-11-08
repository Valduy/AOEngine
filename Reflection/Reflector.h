#pragma once

#include <unordered_map>
#include <cassert>

#include "Type.h"

namespace aoe {

class Reflector {
public:
	template<typename T>
	static const void Register(Type* type) {
		reflector_.Register<T>(type);
	}

	template<typename T>
	static const Type* GetType() {
		auto id = Identifier::GetTypeId<T>();
		return GetType(id);
	}

	static const Type* GetType(TypeId id) {
		return reflector_.GetType(id);
	}

	static const Type* GetType(const std::string& name) {
		return reflector_.GetType(name);
	}

private:
	class ReflectorInternal {
	public:
		~ReflectorInternal() {
			for (auto& it : id_to_type_) {
				delete it.second;
			}

			id_to_type_.clear();
		}

		template<typename T>
		const void Register(Type* type) {
			TypeId type_id = type->GetTypeId();
			auto it = id_to_type_.find(type_id);
			assert(it == id_to_type_.end() && "Type already registered");

			id_to_type_[type->GetTypeId()] = type;
		}

		const Type* GetType(TypeId type_id) {
			auto it = id_to_type_.find(type_id);
			assert(it != id_to_type_.end() && "Type is not registered.");
			return it->second;
		}

		const Type* GetType(const std::string& name) {
			for (const auto& [key, value] : id_to_type_) {
				if (value->GetName() == name) {
					return value;
				}
			}

			return nullptr;
		}

	private:
		std::unordered_map<TypeId, Type*> id_to_type_;
	};

	static inline ReflectorInternal reflector_;
};

} // namespace aoe