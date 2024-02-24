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

const Vector2 Math::kV2Zero = mathfu::kZeros2f;
const Vector2 Math::kV2Ones = mathfu::kOnes2f;
const Vector2 Math::kV2AxisX = mathfu::kAxisX2f;
const Vector2 Math::kV2AxisY = mathfu::kAxisY2f;

const Vector3 Math::kV3Zero = mathfu::kZeros3f;
const Vector3 Math::kV3Ones = mathfu::kOnes3f;
const Vector3 Math::kV3AxisX = mathfu::kAxisX3f;
const Vector3 Math::kV3AxisY = mathfu::kAxisY3f;
const Vector3 Math::kV3AxisZ = mathfu::kAxisZ3f;

const Vector4 Math::kV4Zero = mathfu::kZeros4f;
const Vector4 Math::kV4Ones = mathfu::kOnes4f;
const Vector4 Math::kV4AxisX = mathfu::kAxisX4f;
const Vector4 Math::kV4AxisY = mathfu::kAxisY4f;
const Vector4 Math::kV4AxisZ = mathfu::kAxisZ4f;
const Vector4 Math::kV4AxisW = mathfu::kAxisW4f;

const Quaternion Math::kQIdentity = mathfu::kQuatIdentityf;

} // namespace aoe