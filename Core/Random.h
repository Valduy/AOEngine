#pragma once

#include <random>

#include "Debug.h"
#include "Math.h"

namespace aoe {

class Random {
public:
	using Engine = std::mt19937;

	Random()
		: device_()
		, generator_()
		, int_distributor_()
		, float_distributor_(0.0f, 1.0f)
	{}

	Random(int seed)
		: device_()
		, generator_(static_cast<IntType>(seed))
		, int_distributor_()
		, float_distributor_(0.0f, 1.0f)
	{}

	Engine& GetEngine() {
		return generator_;
	}

	// Return random value.
	int Next() {
		IntType raw_random = int_distributor_(generator_);
		int random = Math::Abs(static_cast<int>(raw_random));
		return random;
	}

	// Return random value from half-interval [0, max)
	int Next(int max) {
		AOE_ASSERT_MSG(max > 0, "Max must be greater than 0.");
		return Next(0, max);
	}

	// Return random value from half-interval [min, max)
	int Next(int min, int max) {
		AOE_ASSERT_MSG(max > min, "Max must be greater than min.");
		int length = max - min;
		int random = Next();
		return min + (random % length);
	}

	float NextFloat() {
		return float_distributor_(generator_);
	}

private:
	using IntType = Engine::result_type;
	
	std::random_device device_;
	Engine generator_;
	std::uniform_int_distribution<IntType> int_distributor_;
	std::uniform_real_distribution<float> float_distributor_;
};

} // namespace aoe