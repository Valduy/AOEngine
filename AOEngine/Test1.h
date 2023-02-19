#pragma once

#include "Preregistrator.h"

class Test1 {
AOE_REFLECTION_BEGIN(Test1)
AOE_REFLECTION_FIELD(Number)
AOE_REFLECTION_END()

public:
	size_t Number;
};