#pragma once

#include <mathfu/constants.h>

namespace aoe {

using Vector3 = mathfu::Vector<float, 3>;
using Vector4 = mathfu::Vector<float, 4>;

using Matrix3 = mathfu::Matrix<float, 3, 3>;
using Matrix4 = mathfu::Matrix<float, 4, 4>;

using Quaternion = mathfu::Quaternion<float>;

class Math {
public:
	static constexpr float kPi = mathfu::kPi;

	Math() = delete;
};

} // namespace aoe