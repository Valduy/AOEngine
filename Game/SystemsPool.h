#pragma once

#include <vector>

#include "ECSSystemBase.h"

namespace aoe {

class SystemsPool {
public:
	~SystemsPool() {
		for (const ECSSystemBase* system : systems_) {
			delete system;
		}

		systems_.clear();
	}

	template<typename TSystem, typename...TParams>
	void PushSystem(TParams&&... params) {
		systems_.push_back(new TSystem(std::forward<TParams>(params)...));
	}

	void Initialize(const aoe::ServiceProvider& service_provider) {
		for (ECSSystemBase* system : systems_) {
			system->Initialize(service_provider);
		}
	}

	void Terminate() {
		for (ECSSystemBase* system : systems_) {
			system->Terminate();
		}
	}

	void PerTickUpdate(float dt) {
		for (ECSSystemBase* system : systems_) {
			system->PerTickUpdate(dt);
		}
	}

	void PerFrameUpdate(float dt) {
		for (ECSSystemBase* system : systems_) {
			system->PerFrameUpdate(dt);
		}
	}

private:
	std::vector<ECSSystemBase*> systems_;
};

} // namespace aoe