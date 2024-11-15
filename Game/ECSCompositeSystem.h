#pragma once

#include "ECSSystemBase.h"
#include "SystemsPool.h"

namespace aoe {

class ECSCompositeSystem : public ECSSystemBase {
public:
	template<typename TSystem, typename...TParams>
	ECSCompositeSystem& PushSystem(TParams&&... params) {
		systems_pool_.PushSystem<TSystem>(std::forward<TParams>(params)...);
		return *this;
	}

	void Initialize(const aoe::ServiceProvider& service_provider) override {
		systems_pool_.Initialize(service_provider);
	}

	void Terminate() override {
		systems_pool_.Terminate();
	}

	void Update(float dt) override {
		systems_pool_.Update(dt);
	}

private:
	SystemsPool systems_pool_;
};

} // namespace aoe