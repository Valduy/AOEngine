#pragma once

#include <map>
#include <cassert>

#include "Type.h"

namespace aoe {

class Reflector {
public:
	template<typename T>
	static const Type& Register(Type* type) {
		return reflector_.Register<T>(type);
	}

	template<typename T>
	static const Type& GetType() {
		auto id = Identifier::GetTypeId<T>();
		return reflector_.GetType(id);
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
		const Type& Register(Type* type) {
			TypeId type_id = type->GetTypeId();
			auto it = id_to_type_.find(type_id);
			assert(it == id_to_type_.end() && "Type already registered");

			id_to_type_[type->GetTypeId()] = type;
			return *type;
		}

		const Type& GetType(TypeId type_id) {
			auto it = id_to_type_.find(type_id);
			assert(it != id_to_type_.end() && "Type is not registered.");
			return *it->second;
		}

	private:
		std::map<TypeId, Type*> id_to_type_;
	};

	static inline ReflectorInternal reflector_;
};

} // namespace aoe