#pragma once

#include <mathfu/constants.h>

namespace aoe {

using Vector2f = mathfu::Vector<float, 2>;
using Vector3f = mathfu::Vector<float, 3>;
using Vector4f = mathfu::Vector<float, 4>;

using Vector2i = mathfu::Vector<int, 2>;
using Vector3i = mathfu::Vector<int, 3>;
using Vector4i = mathfu::Vector<int, 4>;

using Matrix3f = mathfu::Matrix<float, 3, 3>;
using Matrix4f = mathfu::Matrix<float, 4, 4>;

using Quaternion = mathfu::Quaternion<float>;

class Math {
public:
	static constexpr float kPi = 3.141592654f;
	static constexpr float k2Pi = 6.283185307f;
	static constexpr float kPiDiv2 = 1.570796327f;
	static constexpr float kPiDiv4 = 0.785398163f;

	static constexpr float kDeg2Rad = kPi / 180.0f;
	static constexpr float kRad2Deg = 180.0f / kPi;

	static constexpr float kRH = 1.0f;
	static constexpr float kLH = -1.0f;

	static constexpr float kEpsilon = std::numeric_limits<float>::epsilon();

	static const Vector2f kZeros2f;
	static const Vector2f kOnes2f;
	static const Vector2f kAxisX2f;
	static const Vector2f kAxisY2f;

	static const Vector3f kZeros3f;
	static const Vector3f kOnes3f;
	static const Vector3f kAxisX3f;
	static const Vector3f kAxisY3f;
	static const Vector3f kAxisZ3f;
	static const Vector3f kRight;
	static const Vector3f kLeft;
	static const Vector3f kUp;
	static const Vector3f kDown;
	static const Vector3f kForward;
	static const Vector3f kBackward;

	static const Vector4f kZeros4f;
	static const Vector4f kOnes4f;
	static const Vector4f kAxisX4f;
	static const Vector4f kAxisY4f;
	static const Vector4f kAxisZ4f;
	static const Vector4f kAxisW4f;

	static const Vector2i kZeros2i;
	static const Vector2i kOnes2i;
	static const Vector2i kAxisX2i;
	static const Vector2i kAxisY2i;

	static const Vector3i kZeros3i;
	static const Vector3i kOnes3i;
	static const Vector3i kAxisX3i;
	static const Vector3i kAxisY3i;
	static const Vector3i kAxisZ3i;

	static const Vector4i kZeros4i;
	static const Vector4i kOnes4i;
	static const Vector4i kAxisX4i;
	static const Vector4i kAxisY4i;
	static const Vector4i kAxisZ4i;
	static const Vector4i kAxisW4i;

	static const Quaternion kQIdentity;

	Math() = delete;

	template<typename T>
	static T Clamp(T value, T min, T max);

	template<typename T>
	static int Sign(T value);

	static bool Equal(float lhs, float rhs, float epsilon = kEpsilon);
	static float Abs(float value);
	static float Min(float lhs, float rhs);
	static float Max(float lhs, float rhs);
	static float Sin(float angle);
	static float Cos(float angle);
};

template<typename T>
static T Math::Clamp(T value, T min, T max) {
	if (value < min) {
		return min;
	}
	if (value > max) {
		return max;
	}

	return value;
}

template<typename T>
static int Math::Sign(T value) {
	return (0 < value) - (value < 0);
}

} // namespace aoe