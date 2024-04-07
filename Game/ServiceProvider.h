#pragma once

#include <unordered_map>

#include "../Core/Identifier.h"

namespace aoe {

class ServiceProvider {
public:
	template<typename TInterface>
	void AddService(TInterface* implementation) {
		TypeId type_id = Identifier::GetTypeId<TInterface>();
		services_[type_id] = implementation;
	}

	template<typename TInterface>
	TInterface* GetService() const {
		TypeId type_id = Identifier::GetTypeId<TInterface>();
		auto it = services_.find(type_id);
		return it == services_.end() ? nullptr : static_cast<TInterface*>(it->second);
	}

private:
	std::unordered_map<TypeId, void*> services_;
};

} // namespace aoe