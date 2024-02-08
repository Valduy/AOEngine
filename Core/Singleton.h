#pragma once

#include "ClassHelper.h"

namespace aoe {

template<typename T>
class Singleton {
AOE_NON_COPYABLE_AND_NON_MOVABLE_CLASS(Singleton<T>)

public:
	static T* Instance() {
		static T instance;
		return &instance;
	}

protected:
	Singleton() {}
	~Singleton() {}
};

} // namespace aoe