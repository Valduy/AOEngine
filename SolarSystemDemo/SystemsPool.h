#pragma once

#include <vector>

#include "IECSSystem.h"

class SystemsPool {
public:
	~SystemsPool() {
		for (const IECSSystem* system : systems_) {
			delete system;
		}

		systems_.clear();
	}

	template<typename TSystem, typename...TParams>
	void PushSystem(TParams... params) {
		systems_.push_back(new TSystem(params...));
	}

	void Initialize() {
		for (IECSSystem* system : systems_) {
			system->Initialize();
		}
	}

	void Terminate() {
		for (IECSSystem* system : systems_) {
			system->Terminate();
		}
	}

	void PerTickUpdate(float dt) {
		for (IECSSystem* system : systems_) {
			system->PerTickUpdate(dt);
		}
	}

	void PerFrameUpdate(float dt) {
		for (IECSSystem* system : systems_) {
			system->PerFrameUpdate(dt);
		}
	}

private:
	std::vector<IECSSystem*> systems_;
};