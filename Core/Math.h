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

	static bool IsEqual(float lhs, float rhs, float epsilon = kEpsilon);
	static bool IsEqual(const Vector2f& lhs, const Vector2f& rhs, float epsilon = kEpsilon);

	template<typename T>
	static T Clamp(T value, T min, T max);

	template<typename T>
	static int Sign(T value);

	template<typename T>
	static T Abs(T value);
	
	template<typename T>
	static T Min(T lhs, T rhs);

	template<typename T>
	static T Max(T lhs, T rhs);

	static float Pow(float value, float power);
	static float Ceil(float value);
	static float Floor(float value);
	static int CeilToInt(float value);
	static int FloorToInt(float value);
	static float Lerp(float a, float b, float t);
	static float Sqrt(float value);
	static float Sin(float angle);
	static float Cos(float angle);
	static float Acos(float cos);
	static float Asin(float sin);
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

template<typename T>
static T Math::Abs(T value) {
	return value >= 0 ? value : -value;
}

template<typename T>
static T Math::Min(T lhs, T rhs) {
	return lhs < rhs ? lhs : rhs;
}

template<typename T>
static T Math::Max(T lhs, T rhs) {
	return lhs > rhs ? lhs : rhs;
}

} // namespace aoe