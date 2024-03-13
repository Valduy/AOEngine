#pragma once

#include <mathfu/constants.h>

namespace aoe {

using Vector2 = mathfu::Vector<float, 2>;
using Vector3 = mathfu::Vector<float, 3>;
using Vector4 = mathfu::Vector<float, 4>;

using Matrix3 = mathfu::Matrix<float, 3, 3>;
using Matrix4 = mathfu::Matrix<float, 4, 4>;

using Quaternion = mathfu::Quaternion<float>;

class Math {
public:
	static constexpr float kPi = mathfu::kPi;

	static const Vector2 kV2Zero;
	static const Vector2 kV2Ones;
	static const Vector2 kV2AxisX;
	static const Vector2 kV2AxisY;

	static const Vector3 kV3Zero;
	static const Vector3 kV3Ones;
	static const Vector3 kV3AxisX;
	static const Vector3 kV3AxisY;
	static const Vector3 kV3AxisZ;

	static const Vector4 kV4Zero;
	static const Vector4 kV4Ones;
	static const Vector4 kV4AxisX;
	static const Vector4 kV4AxisY;
	static const Vector4 kV4AxisZ;
	static const Vector4 kV4AxisW;

	static const Quaternion kQIdentity;

	Math() = delete;
};

} // namespace aoe