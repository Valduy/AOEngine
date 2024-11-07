#pragma once

#include "../Core/ClassHelper.h"

#include <vector>

#include "ECSSystemBase.h"

namespace aoe {

class SystemsPool {
AOE_NON_COPYABLE_AND_NON_MOVABLE_CLASS(SystemsPool)

public:
	SystemsPool()
		: systems_()
		, is_inited_(false)
	{}

	~SystemsPool() {
		for (const ECSSystemBase* system : systems_) {
			delete system;
		}

		systems_.clear();
	}

	template<typename TSystem, typename...TParams>
	void PushSystem(TParams&&... params) {
		AOE_ASSERT_MSG(!is_inited_, "Systems already inited.");

		TSystem* system = new TSystem(std::forward<TParams>(params)...);
		systems_.push_back(system);
	}

	void Initialize(const aoe::ServiceProvider& service_provider) {
		AOE_ASSERT_MSG(!is_inited_, "Systems already inited.");

		for (ECSSystemBase* system : systems_) {
			system->Initialize(service_provider);
		}

		is_inited_ = true;
	}

	void Terminate() {
		for (ECSSystemBase* system : systems_) {
			system->Terminate();
		}

		is_inited_ = false;
	}

	void Update(float dt) {
		AOE_ASSERT_MSG(is_inited_, "Systems is not inited.");

		for (ECSSystemBase* system : systems_) {
			system->Update(dt);
		}
	}

private:
	std::vector<ECSSystemBase*> systems_;
	bool is_inited_;
};

} // namespace aoe